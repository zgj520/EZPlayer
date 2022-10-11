#include "FFBaseDecoder.h"
#include "GPUDeviceManger.h"
#include <vector>
#include <functional>

static void* threadFunc(void* arg) {
    FFBaseDecoder* decode = static_cast<FFBaseDecoder*>(arg);
    decode->decodeThreadFunc();
    return nullptr;
}

FFBaseDecoder::FFBaseDecoder(const std::string& filePath):m_filePath(filePath) {

}

FFBaseDecoder::~FFBaseDecoder() {
    m_bStop = true;
    if (m_pThread) {
        m_frameCacheConditionVar.notify_all();
        m_pThread->join();
        delete m_pThread;
    }
    clearCache();
    av_buffer_unref(&m_pCodecContext->hw_device_ctx);
    avcodec_free_context(&m_pCodecContext);
    avformat_close_input(&m_pFormatContext);
}

bool FFBaseDecoder::init() {
    if (isValid()) {
        m_pThread = new std::thread(&threadFunc, this);
        return true;
    }
    return false;
}

AVFrame* FFBaseDecoder::getOneFrame(int64_t& timesample) {
    {
        std::unique_lock<std::mutex> uniqueLock(m_frameCacheMutex);
        if (!m_vtFrameCache.empty()) {
            auto frame = m_vtFrameCache[0];
            m_vtFrameCache.erase(m_vtFrameCache.begin());
            m_frameCacheConditionVar.notify_one();
            timesample = frame->pts * av_q2d((*m_pFormatContext->streams)->time_base) * 1000 * 1000;
            return frame;
        }
        if (m_isEOF) {
            return nullptr;
        }
    }
    m_frameCacheConditionVar.notify_one();
    {
        std::condition_variable getFrameConditionVar;
        std::mutex getFrameMutex;
        std::unique_lock<std::mutex> uniqueLock(getFrameMutex);
        while (!m_bStop) {
            using namespace std::chrono_literals;
            getFrameConditionVar.wait_for(uniqueLock, 5ms, [this]()->bool {
                std::unique_lock<std::mutex> uniqueLock(m_frameCacheMutex);
                return !m_vtFrameCache.empty() || m_isEOF;
                });
            std::unique_lock<std::mutex> uniqueLock(m_frameCacheMutex);
            if (!m_vtFrameCache.empty()) {
                auto frame = m_vtFrameCache[0];
                m_vtFrameCache.erase(m_vtFrameCache.begin());
                getFrameConditionVar.notify_one();
                timesample = frame->pts * av_q2d((*m_pFormatContext->streams)->time_base) * 1000 * 1000;
                return frame;
            }
        }
    }
    return nullptr;
}

bool FFBaseDecoder::getMediaInfo(EZCore::MediaInfo& info) {
    if (!isValid()) {
        return false;
    }
    info.duration = m_pFormatContext->duration/*/(AV_TIME_BASE/1000)*/;
    AVStream** stream = (m_pFormatContext->streams);
    int den = (*stream)->avg_frame_rate.den;
    int num = (*stream)->avg_frame_rate.num;
    info.fps = (float)num / (float)den;

    int msnum = (m_pFormatContext->duration % 1000000) / 1000;
    int yuzhen = (int)(msnum * 1 / info.fps);
    info.frameCount = (m_pFormatContext->duration / 1000000) * (int)info.fps + yuzhen;
    return true;
}

bool FFBaseDecoder::seekTime(int64_t dsttime) {
    if (!isValid()) {
        return false;
    }
    int ret = 0;
    {
        std::unique_lock<std::mutex> uniqueLock(m_decodeMutex);
        avcodec_flush_buffers(m_pCodecContext);
        AVStream* stream = *(m_pFormatContext->streams);
        auto time_base = stream->time_base;
        auto seekTime = av_rescale_q(dsttime, AVRational{ 1, AV_TIME_BASE }, time_base);
        int ret = avformat_seek_file(m_pFormatContext, stream->index, INT_MIN, seekTime, INT_MAX, AVSEEK_FLAG_BACKWARD);
        if (ret < 0) {
            ret = av_seek_frame(m_pFormatContext, stream->index, seekTime, AVSEEK_FLAG_BACKWARD);
        }
    }
    if (ret >= 0) {
        clearCache();
        m_isEOF = false;
    }
    return ret >= 0;
}

bool FFBaseDecoder::isEOF() {
    return m_isEOF;
}

void FFBaseDecoder::freeOneFrame(AVFrame* frame) {
    av_frame_free(&frame);
}

bool FFBaseDecoder::isValid() {
    if (m_pFormatContext == nullptr || m_pCodecContext == nullptr) {
        return false;
    }
    return true;
}

void FFBaseDecoder::clearCache() {
    std::unique_lock<std::mutex> uniqueLock(m_frameCacheMutex);
    for (auto frame : m_vtFrameCache) {
        freeOneFrame(frame);
    }
    m_vtFrameCache.clear();
}

void FFBaseDecoder::decodeThreadFunc() {
    while (!m_bStop) {
        std::unique_lock<std::mutex> decodeLock(m_decodeMutex);
        AVPacket* packet = av_packet_alloc();
        if (av_read_frame(m_pFormatContext, packet) < 0) {
            std::unique_lock<std::mutex> uniqueLock(m_frameCacheMutex);
            m_isEOF = true;
            decodeLock.unlock();
            m_frameCacheConditionVar.wait(uniqueLock);
            decodeLock.lock();
            continue;
        }

        m_isEOF = false;
        if (m_StreamIndex != packet->stream_index) {
            av_packet_free(&packet);
            continue;
        }

        int ret = avcodec_send_packet(m_pCodecContext, packet);
        if (ret < 0) {
            av_packet_free(&packet);
            break;
        }
        {
            std::unique_lock<std::mutex> uniqueLock(m_frameCacheMutex);
            if (m_vtFrameCache.size() >= 3) {
                decodeLock.unlock();
                m_frameCacheConditionVar.wait(uniqueLock);
                decodeLock.lock();
            }
        }
        while (!m_bStop) {
            AVFrame* pFrame = av_frame_alloc();
            if (pFrame == nullptr) {
                break;
            }

            ret = avcodec_receive_frame(m_pCodecContext, pFrame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                av_frame_free(&pFrame);
                break;
            }
            if (ret < 0) {
                av_frame_free(&pFrame);
                break;
            }
            if (m_pCodecContext->hw_device_ctx) {
                AVFrame* sw_frame = av_frame_alloc();
                av_hwframe_transfer_data(sw_frame, pFrame, 0);
                sw_frame->pts = pFrame->pts;
                m_vtFrameCache.push_back(sw_frame);
                av_frame_free(&pFrame);
            }
            else {
                m_vtFrameCache.push_back(pFrame);
            }
        }
        av_packet_free(&packet);
    }
    return;
}