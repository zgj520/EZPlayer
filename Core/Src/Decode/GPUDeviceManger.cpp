#include "GPUDeviceManger.h"
#include "String/StringUtils.h"
#include <windows.h>
#include <VersionHelpers.h>
#include <dxgi.h>

extern "C" {
#include "libavutil/pixfmt.h"
}

const unsigned int Nvidia_vendorid = 4318;
const unsigned int Intel_vendorid = 32902;
const unsigned int AMD_vendorid = 4098;
const unsigned int MircsoftBasic_vendorid = 5140;


#pragma comment(lib, "DXGI.lib")

GPUDeviceManger::GPUDeviceManger() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    init();
}

void GPUDeviceManger::init() {
    IDXGIFactory1* pDXGIFactory1 = nullptr;
    IDXGIAdapter1* pDXGIAdapter1 = nullptr;
    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pDXGIFactory1));
    if (FAILED(hr)) {
        return;
    }

    int nAdapterIndex = 0;
    while (pDXGIFactory1->EnumAdapters1(nAdapterIndex, &pDXGIAdapter1) != DXGI_ERROR_NOT_FOUND) {
        DXGI_ADAPTER_DESC1 desc;
        hr = pDXGIAdapter1->GetDesc1(&desc);
        if (hr != S_OK) {
            nAdapterIndex++;
            continue;
        }
        GPUDevice device;
        if (desc.VendorId == Nvidia_vendorid) {
            device.vendor = Nvidia;
        }
        else if (desc.VendorId == Intel_vendorid) {
            device.vendor = Intel;
        }
        else if(desc.VendorId == AMD_vendorid){
            device.vendor = AMD;
        }
        else{
            device.vendor = Unkown;
            nAdapterIndex++;
            continue;
        }
        device.gpuName = wstring2String(desc.Description);
        device.index = nAdapterIndex;
        getGPUEncodeRecommend(device, device.encodeRecommend);
        getGPUDecodeRecommend(device, device.decodeRecommend);
        m_vtGpu.push_back(device);
        nAdapterIndex++;
    }
    if (pDXGIAdapter1 != nullptr) {
        pDXGIAdapter1->Release();
    }
    if (pDXGIFactory1 != nullptr) {
        pDXGIFactory1->Release();
    }
}

bool GPUDeviceManger::getGPUEncodeRecommend(const GPUDevice& device, EncodeRecommend &encodeRecommend)
{
    if (device.vendor == Unkown) {
        return false;
    }
    bool bWin10 = currentIsWin10Systerm();
    if (device.vendor == Nvidia)
    {
        encodeRecommend.deviceName = bWin10 ? "d3d11va" : "cuda";
        encodeRecommend.deviceIndex = std::to_string(device.index);
        encodeRecommend.encodeName = "h264_nvenc";
        // AV_PIX_FMT_CUDA,送CUDA格式数据需要转换成GPU格式然后送，NV12格式ffmpeg会帮忙转，但是CUDA格式编码性能好
        encodeRecommend.hw_input_pix_format = encodeRecommend.deviceName == "d3d11va" ? AV_PIX_FMT_D3D11 : AV_PIX_FMT_CUDA;//AV_PIX_FMT_NV12;
    }
    else if (device.vendor == Intel) 
    {
        encodeRecommend.deviceName = "qsv";
        encodeRecommend.deviceIndex = "hw_any";
        encodeRecommend.encodeName = "h264_qsv";
        encodeRecommend.hw_input_pix_format = AV_PIX_FMT_NV12;
    }
    else if(device.vendor == AMD)
    {
        encodeRecommend.deviceName = "amf";
        encodeRecommend.deviceIndex = std::to_string(device.index);
        encodeRecommend.encodeName = "h264_amf";
    }
    else
    {
        return false;
    }
    return true;
}

bool GPUDeviceManger::getGPUDecodeRecommend(const GPUDevice& device, DeocdeRecommend &decodeRecommend)
{
    if (device.vendor == Unkown) {
        return false;
    }
    bool bWin10 = currentIsWin10Systerm();
    if (device.vendor == Nvidia)
    {
        decodeRecommend.deviceName = bWin10 ? "d3d11va" : "cuda";
        decodeRecommend.deviceIndex = std::to_string(device.index);
    }
    else if (device.vendor == Intel)
    {
        decodeRecommend.deviceName = bWin10 ? "d3d11va" : "qsv";
        decodeRecommend.deviceIndex = decodeRecommend.decodeName == "d3d11va"?std::to_string(device.index):"auto";
        decodeRecommend.decodeName = decodeRecommend.decodeName == "d3d11va" ? "" : "h264_qsv";
    }
    else if (device.vendor == AMD)
    {
        decodeRecommend.deviceName = bWin10 ? "d3d11va" : "dxva2";
        decodeRecommend.deviceIndex = std::to_string(device.index);
    }
    else
    {
        return false;
    }
    return true;
}

GPUDeviceManger::~GPUDeviceManger() {
    CoUninitialize();
}

bool GPUDeviceManger::currentIsWin10Systerm() {
    return IsWindows8OrGreater();
}