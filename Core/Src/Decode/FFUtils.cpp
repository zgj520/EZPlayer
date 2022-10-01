#include "FFUtils.h"
#include "../Interface/CoreInterface.h"

namespace EZCore {

    bool FFUtils::getMediaInfo(const std::string& filePath, MediaInfo& info) {
        AVFormatContext* pFormatContext = avformat_alloc_context();
        if (pFormatContext == nullptr) {
            return false;
        }

        int ret = avformat_open_input(&pFormatContext, filePath.c_str(), NULL, NULL);
        if (ret < 0) {
            return false;
        }

        ret = avformat_find_stream_info(pFormatContext, NULL);
        if (ret < 0) {
            return false;
        }

        AVCodec* pCodec = nullptr;
        auto videoStreamIndex = av_find_best_stream(pFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &pCodec, 0);
        if (videoStreamIndex < 0 || pCodec == nullptr) {
            return false;
        }
        auto pVideoStream = pFormatContext->streams[videoStreamIndex];
        
        AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
        if (pCodecCtx == nullptr) {
            return false;
        }
        ret = avcodec_parameters_to_context(pCodecCtx, pVideoStream->codecpar);
        if (ret < 0) {
            return false;
        }
        info.duration = pFormatContext->duration;
        info.width = pCodecCtx->width;
        info.height = pCodecCtx->height;
        auto avFPS = pFormatContext->streams[videoStreamIndex]->avg_frame_rate;
        info.fps = 1.0* avFPS.num/ avFPS.den;

        av_reduce(&info.displayWidth, &info.displayHeight, pCodecCtx->width * (int64_t)pVideoStream->sample_aspect_ratio.num, pCodecCtx->height * (int64_t)pVideoStream->sample_aspect_ratio.den, 1024 * 1024);

        avcodec_free_context(&pCodecCtx);
        avformat_close_input(&pFormatContext);
        return true;
    }
}
