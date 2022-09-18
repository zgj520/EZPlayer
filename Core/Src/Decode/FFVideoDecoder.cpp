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
    av_buffer_unref(&m_pCodecContext->hw_device_ctx);
    avcodec_free_context(&m_pCodecContext);
    avformat_close_input(&m_pFormatContext);
}

AVFrame* FFVideoDecoder::getOneFrame() {
    {
        std::unique_lock<std::mutex> uniqueLock(m_decodeMutex);
        if (!m_vtFrameCache.empty()) {
            auto frame = m_vtFrameCache[0];
            m_vtFrameCache.erase(m_vtFrameCache.begin());
            m_decodeConditionVar.notify_one();
            return frame;
        }
        if (m_isEOF) {
            return nullptr;
        }
    }
    m_decodeConditionVar.notify_one();
    {
        std::unique_lock<std::mutex> uniqueLock(m_getFrameMutex);
        while (!m_bStop) {
            using namespace std::chrono_literals;
            m_getFrameConditionVar.wait_for(uniqueLock, 5ms, [this]()->bool {
                std::unique_lock<std::mutex> uniqueLock(m_decodeMutex);
                return !m_vtFrameCache.empty() || m_isEOF;
            });
            std::unique_lock<std::mutex> uniqueLock(m_decodeMutex);
            if (!m_vtFrameCache.empty()) {
                auto frame = m_vtFrameCache[0];
                m_vtFrameCache.erase(m_vtFrameCache.begin());
                m_decodeConditionVar.notify_one();
                return frame;
            }
        }
    }
    return nullptr;
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

void FFVideoDecoder::decodeThreadFunc() {
    AVPacket packet;
    while (!m_bStop) {
        if (av_read_frame(m_pFormatContext, &packet) < 0) {
            std::unique_lock<std::mutex> uniqueLock(m_decodeMutex);
            m_isEOF = true;
            m_decodeConditionVar.wait(uniqueLock);
            continue;
        }

        m_isEOF = false;
        if (m_videoStreamIndex != packet.stream_index) {
            av_packet_unref(&packet);
            continue;
        }

        int ret = avcodec_send_packet(m_pCodecContext, &packet);
        if (ret < 0) {
            break;
        }
        {
            std::unique_lock<std::mutex> uniqueLock(m_decodeMutex);
            if (m_vtFrameCache.size() >= 3) {
                m_decodeConditionVar.wait(uniqueLock);
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
        av_packet_unref(&packet);
    }
    packet.data = NULL;
    packet.size = 0;
    av_packet_unref(&packet);
    return;
}