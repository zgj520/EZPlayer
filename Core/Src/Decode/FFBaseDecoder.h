#ifndef FFBASEDECODE_H_
#define FFBASEDECODE_H_
#include <string>
#include <thread>
#include <mutex>
#include <vector>
extern "C" {
#include "libavutil/hwcontext.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}
#include "../Interface/CoreInterface.h"

class FFBaseDecoder
{
public:
    FFBaseDecoder(const std::string& filePath);
    ~FFBaseDecoder();

    AVFrame* getOneFrame(int64_t &timesample);

    bool seekTime(int64_t dsttime);

    bool getMediaInfo(EZCore::MediaInfo& info);

    bool isEOF();

    void freeOneFrame(AVFrame*);

    void decodeThreadFunc();

protected:
    virtual bool init();
    bool isValid();
    void clearCache();

protected:
    std::string m_filePath = "";
    AVCodecContext* m_pCodecContext = nullptr;
    AVCodec* m_pCodec = nullptr;
    AVFormatContext* m_pFormatContext = nullptr;
    int m_StreamIndex = -1;

    std::thread* m_pThread = nullptr;
    std::atomic_bool m_bStop = false;
    std::atomic_bool m_isEOF = { false };

    std::condition_variable m_frameCacheConditionVar;
    std::mutex m_frameCacheMutex;
    std::vector<AVFrame*> m_vtFrameCache;

    std::mutex m_decodeMutex;
};

#endif