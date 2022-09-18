#ifndef GPUDEVICEMANGER_H_
#define GPUDEVICEMANGER_H_
#include "Singleton/Singleton.h"
#include <string>
#include <vector>

enum GPUVendor : int
{
    Unkown,
    Nvidia,
    Intel,
    AMD,
    GPUVendor_Max
};

struct EncodeRecommend
{
    std::string deviceName;
    std::string encodeName;
    std::string deviceIndex;
    int hw_input_pix_format = -1;
};

struct DeocdeRecommend
{
    std::string deviceName;
    std::string decodeName;
    std::string deviceIndex;
};

struct GPUDevice
{
    GPUVendor vendor = Unkown;
    std::string gpuName = "";
    int index = 0;
    EncodeRecommend encodeRecommend;
    DeocdeRecommend decodeRecommend;
};

class GPUDeviceManger:public Singleton<GPUDeviceManger>
{
    friend class Singleton<GPUDeviceManger>;

public:
    void init();

    bool getGPUEncodeRecommend(const GPUDevice &device, EncodeRecommend &encodeRecommend);

    bool getGPUDecodeRecommend(const GPUDevice &decice, DeocdeRecommend &decodeRecommend);

private:
    GPUDeviceManger();
    ~GPUDeviceManger();

private:
    bool currentIsWin10Systerm();

private:
    std::vector<GPUDevice> m_vtGpu;
};

#endif