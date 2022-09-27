#ifndef FFUTILS_H_
#define FFUTILS_H_
#include "FFVideoDecoder.h"
#include "../Utils/Src/Singleton/Singleton.h"
extern "C" {
#include "libavutil/hwcontext.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

namespace EZCore {
    class MediaInfo;
    class FFUtils :public Singleton<FFUtils>{

    public:
        bool getMediaInfo(const std::string& filePath, MediaInfo& info);
    };
}

#endif