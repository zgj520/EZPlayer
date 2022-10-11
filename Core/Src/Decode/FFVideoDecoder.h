#ifndef FFVIDEODECODE_H_
#define FFVIDEODECODE_H_
#include "FFBaseDecoder.h"

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

class FFVideoDecoder : public FFBaseDecoder
{
public:
    FFVideoDecoder(const std::string& filePath);
    ~FFVideoDecoder();

    void decodeThreadFunc();

private:
    bool init() override;

    bool m_bForceUseSoftDecode = false;
};

#endif