#include "FFVideoDecoder.h"
#include "GPUDeviceManger.h"
#include <vector>
#include <functional>

static void* threadFunc(void* arg) {
    FFVideoDecoder* decode = static_cast<FFVideoDecoder*>(arg);
    decode->decodeThreadFunc();
    return nullptr;
}

static AVPixelFormat hw_decode_pix_fmt = AV_PIX_FMT_NONE;
enum AVPixelFormat get_hw_format(struct AVCodecContext *s, const enum AVPixelFormat * fmt)
{
    auto pFormat = fmt;
    while (*pFormat != AV_PIX_FMT_NONE){
        if (*pFormat == hw_decode_pix_fmt) {
            return *pFormat;
        }
        pFormat++;
    }
    return AV_PIX_FMT_NONE;
}

bool convertFrame2DstFormat(AVFrame* pSrcFrame, AVFrame* dstFrame, AVPixelFormat dstFmt)
{
    if (pSrcFrame == nullptr || dstFrame == nullptr) {
        return false;
    }
    int width = pSrcFrame->width;
    int height = pSrcFrame->height;
    auto pSwsContext = sws_getContext(width, height, (AVPixelFormat)pSrcFrame->format, width, height, dstFmt, SWS_BILINEAR, NULL, NULL, NULL);
    if (pSwsContext == nullptr) {
        return false;
    }
    int ret = sws_scale(pSwsContext, pSrcFrame->data, pSrcFrame->linesize, 0, height, dstFrame->data, dstFrame->linesize);
    if (ret < 0) {
        sws_freeContext(pSwsContext);
        return false;
    }
    dstFrame->format = dstFmt;
    dstFrame->width = width;
    dstFrame->height = height;
    sws_freeContext(pSwsContext);
    return true;
}

FFVideoDecoder::FFVideoDecoder(const std::string& filePath){
    m_filePath = filePath;
    if (initBySoft()) {
        m_pThread = new std::thread(&threadFunc, this);
    }
}

FFVideoDecoder::~FFVideoDecoder(){
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

AVFrame* FFVideoDecoder::getOneFrame(int64_t& timesample) {
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
                timesample = frame->pts * av_q2d((*m_pFormatContext->streams)->time_base)*1000*1000;
                return frame;
            }
        }
    }
    return nullptr;
}

bool FFVideoDecoder::getMediaInfo(EZCore::MediaInfo& info) {
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

bool FFVideoDecoder::seekTime(int64_t dsttime) {
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

bool FFVideoDecoder::isEOF() {
    return m_isEOF;
}

void FFVideoDecoder::freeOneFrame(AVFrame* frame) {
    av_frame_free(&frame);
}

bool FFVideoDecoder::init() {
    HWDevice g_hwdevices[] = {
        { "d3d11va", HWAccel_D3D11,  0 },
        { "cuda",    HWAccel_CUDA,   0 },
        { "cuvid",   HWAccel_CUVID,  0 },
        { "qsv",     HWAccel_QSV,    0 },
        { "dava2",   HWAccel_DXVA2,  0 }
    };

    AVHWDeviceType device_type = av_hwdevice_find_type_by_name("d3d11va");
    if (device_type == AV_HWDEVICE_TYPE_NONE) {
        return false;
    }

    AVFormatContext* pFormatContext = avformat_alloc_context();
    if (pFormatContext == nullptr) {
        return false;
    }

    int ret = avformat_open_input(&pFormatContext, m_filePath.c_str(), NULL, NULL);
    if (ret < 0) {
        return false;
    }

    ret = avformat_find_stream_info(pFormatContext, NULL);
    if (ret < 0) {
        return false;
    }

    AVCodec* pCodec = nullptr;
    m_videoStreamIndex = av_find_best_stream(pFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &pCodec, 0);
    if (m_videoStreamIndex < 0 || pCodec == nullptr) {
        return false;
    }

    for (int i = 0;; i++) {
        const AVCodecHWConfig* pHWConfig = avcodec_get_hw_config(pCodec, i);
        if (pHWConfig == nullptr) {
            break;
        }
        if (pHWConfig->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX && pHWConfig->device_type == device_type) {
            hw_decode_pix_fmt = pHWConfig->pix_fmt;
            break;
        }
    }
    if (hw_decode_pix_fmt == AV_PIX_FMT_NONE) {
        return false;
    }

    AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == nullptr) {
        return false;
    }
    ret = avcodec_parameters_to_context(pCodecCtx, pFormatContext->streams[m_videoStreamIndex]->codecpar);
    if (ret < 0) {
        return false;
    }
    pCodecCtx->get_format = get_hw_format;

    AVBufferRef* hw_device_ctx = nullptr;
    int nGpuIndex = 0;
    for (int i = 0; i < 3; i++) {
        ret = av_hwdevice_ctx_create(&hw_device_ctx, device_type, std::to_string(i).c_str(), NULL, NULL);
        if (ret < 0 || hw_device_ctx == nullptr) {
            continue;
        }
        ret = av_hwdevice_ctx_init(hw_device_ctx);
        break;
    }
    if (hw_device_ctx == nullptr) {
        return false;
    }
    pCodecCtx->hw_device_ctx = av_buffer_ref(hw_device_ctx);

    ret = avcodec_open2(pCodecCtx, pCodec, NULL);
    if (ret < 0) {
        return false;
    }

    m_pCodecContext = pCodecCtx;
    m_pCodec = pCodec;
    m_pFormatContext = pFormatContext;
    return true;
}

bool FFVideoDecoder::initBySoft() {
    AVFormatContext* pFormatContext = avformat_alloc_context();
    if (pFormatContext == nullptr) {
        return false;
    }

    int ret = avformat_open_input(&pFormatContext, m_filePath.c_str(), NULL, NULL);
    if (ret < 0) {
        return false;
    }

    ret = avformat_find_stream_info(pFormatContext, NULL);
    if (ret < 0) {
        return false;
    }

    AVCodec* pCodec = nullptr;
    m_videoStreamIndex = av_find_best_stream(pFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &pCodec, 0);
    if (m_videoStreamIndex < 0 || pCodec == nullptr) {
        return false;
    }

    AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == nullptr) {
        return false;
    }
    ret = avcodec_parameters_to_context(pCodecCtx, pFormatContext->streams[m_videoStreamIndex]->codecpar);
    if (ret < 0) {
        return false;
    }

    ret = avcodec_open2(pCodecCtx, pCodec, NULL);
    if (ret < 0) {
        return false;
    }

    m_pCodecContext = pCodecCtx;
    m_pCodec = pCodec;
    m_pFormatContext = pFormatContext;
    return true;
}

bool FFVideoDecoder::isValid() {
    if (m_pFormatContext == nullptr || m_pCodecContext == nullptr) {
        return false;
    }
    return true;
}

void FFVideoDecoder::clearCache() {
    std::unique_lock<std::mutex> uniqueLock(m_frameCacheMutex);
    for (auto frame: m_vtFrameCache) {
        freeOneFrame(frame);
    }
    m_vtFrameCache.clear();
}

void FFVideoDecoder::decodeThreadFunc() {
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
        if (m_videoStreamIndex != packet->stream_index) {
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
            m_vtFrameCache.push_back(pFrame);
        }
        av_packet_free(&packet);
    }
    return;
}