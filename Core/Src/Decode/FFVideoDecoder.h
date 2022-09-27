#ifndef FFVIDEODECODE_H_
#define FFVIDEODECODE_H_
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

enum HWAccelID{
    HWAccel_QSV,
    HWAccel_CUDA,
    HWAccel_DXVA2,
    HWAccel_D3D11,
    HWAccel_CUVID
};

struct HWDevice{
    const char* device;
    enum HWAccelID id;
    int adapter;
};

class FFVideoDecoder
{
public:
    FFVideoDecoder(const std::string& filePath);
    ~FFVideoDecoder();

    AVFrame* getOneFrame(int64_t &timesample);

    bool seekTime(int64_t dsttime);

    bool getMediaInfo(EZCore::MediaInfo& info);

    bool isEOF();

    void freeOneFrame(AVFrame*);

    void decodeThreadFunc();

private:
    bool init();
    bool initBySoft();
    bool isValid();
    void clearCache();

private:
    std::string m_filePath = "";
    AVCodecContext* m_pCodecContext = nullptr;
    AVCodec* m_pCodec = nullptr;
    AVFormatContext* m_pFormatContext = nullptr;
    int m_videoStreamIndex = -1;

    std::thread* m_pThread = nullptr;
    std::atomic_bool m_bStop = false;
    std::atomic_bool m_isEOF = { false };


    std::condition_variable m_frameCacheConditionVar;
    std::mutex m_frameCacheMutex;
    std::vector<AVFrame*> m_vtFrameCache;

    std::mutex m_decodeMutex;
};

#endif