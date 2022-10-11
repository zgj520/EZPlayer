#include "FFVideoDecoder.h"
#include "GPUDeviceManger.h"
#include <vector>
#include <functional>

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

FFVideoDecoder::FFVideoDecoder(const std::string& filePath): FFBaseDecoder(filePath){
    init();
}

FFVideoDecoder::~FFVideoDecoder(){
}

bool FFVideoDecoder::init() {
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
        avformat_close_input(&m_pFormatContext);
        return false;
    }

    AVCodec* pVideoCodec = nullptr;
    m_StreamIndex = av_find_best_stream(pFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &pVideoCodec, 0);
    if (m_StreamIndex < 0 || pVideoCodec == nullptr) {
        avformat_close_input(&m_pFormatContext);
        return false;
    }
    AVCodecContext* pCodecCtx = avcodec_alloc_context3(pVideoCodec);
    if (pCodecCtx == nullptr) {
        return false;
    }
    ret = avcodec_parameters_to_context(pCodecCtx, pFormatContext->streams[m_StreamIndex]->codecpar);
    if (ret < 0) {
        return false;
    }

    if (!m_bForceUseSoftDecode) {
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
        for (int i = 0;; i++) {
            const AVCodecHWConfig* pHWConfig = avcodec_get_hw_config(pVideoCodec, i);
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
    }

    ret = avcodec_open2(pCodecCtx, pVideoCodec, NULL);
    if (ret < 0) {
        return false;
    }
    m_pCodecContext = pCodecCtx;
    m_pCodec = pVideoCodec;
    m_pFormatContext = pFormatContext;
    FFBaseDecoder::init();
    return true;
}
