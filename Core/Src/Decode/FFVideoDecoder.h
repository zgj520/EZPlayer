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

enum HWAccelID{
    HWAccel_QSV,
    HWAccel_CUDA,
    HWAccel_DXVA2,
    HWAccel_D3D11,
    HWAccel_CUVID
};

struct HWDevice
{
    const char* device;
    enum HWAccelID id;
    int adapter;
};

class FFVideoDecoder
{
public:
    FFVideoDecoder(const std::string& filePath);
    ~FFVideoDecoder();

    AVFrame* getOneFrame();

    bool isEOF();

    void freeOneFrame(AVFrame*);

    void decodeThreadFunc();

private:
    bool init();
    bool initBySoft();

private:
    std::string m_filePath = "";
    AVCodecContext* m_pCodecContext = nullptr;
    AVCodec* m_pCodec = nullptr;
    AVFormatContext* m_pFormatContext = nullptr;
    int m_videoStreamIndex = -1;

    std::thread* m_pThread = nullptr;
    bool m_bStop = false;
    std::condition_variable m_decodeConditionVar;
    std::mutex m_decodeMutex;
    std::atomic_bool m_isEOF = {false};

    std::vector<AVFrame*> m_vtFrameCache;

    std::condition_variable m_getFrameConditionVar;
    std::mutex m_getFrameMutex;
};

#endif