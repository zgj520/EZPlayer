#include "FFAudioDecoder.h"



FFAudioDecoder::FFAudioDecoder(const std::string& filePath): FFBaseDecoder(filePath){
    init();
}

FFAudioDecoder::~FFAudioDecoder(){

}

bool FFAudioDecoder::init() {
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
    m_StreamIndex = av_find_best_stream(pFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &pCodec, 0);
    if (m_StreamIndex < 0 || pCodec == nullptr) {
        return false;
    }

    AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == nullptr) {
        return false;
    }
    ret = avcodec_parameters_to_context(pCodecCtx, pFormatContext->streams[m_StreamIndex]->codecpar);
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
    FFBaseDecoder::init();
    return true;
}