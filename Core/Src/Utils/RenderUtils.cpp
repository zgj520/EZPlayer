#include "RenderUtils.h"
#include <algorithm>
extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/hwcontext_d3d11va.h"
}

bool CopyD3D11AVFrame2D3D11Texture(Microsoft::WRL::ComPtr<ID3D11Texture2D>& dstTexture, ID3D11Device* dstDevice, AVFrame* frame){
    if (frame == nullptr || dstDevice == nullptr) {
        return false;
    }
    ID3D11Texture2D* srcTexture = reinterpret_cast<ID3D11Texture2D*>(frame->data[0]);
    if (srcTexture == nullptr) {
        return false;
    }
    HRESULT ret = S_OK;
    D3D11_TEXTURE2D_DESC desc;
    srcTexture->GetDesc(&desc);

    auto align2 = [](int x) -> int {
        return (x + 1) & -1;
    };
    D3D11_TEXTURE2D_DESC tdesc = {};
    tdesc.Format = desc.Format;
    tdesc.Usage = D3D11_USAGE_DEFAULT;
    tdesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
    tdesc.ArraySize = 1;
    tdesc.MipLevels = 1;
    tdesc.SampleDesc = { 1, 0 };
    tdesc.Height = min(frame->height, align2(frame->height));
    tdesc.Width = min(frame->width, align2(frame->width));
    tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    ret = dstDevice->CreateTexture2D(&tdesc, nullptr, &dstTexture);
    if (ret != S_OK) {
        return false;
    }
    HANDLE sharedHandle;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> sharedTexture = dstTexture;
    Microsoft::WRL::ComPtr<IDXGIResource> dxgiResource;
    sharedTexture.As(&dxgiResource);
    ret = dxgiResource->GetSharedHandle(&sharedHandle);
    if (ret != S_OK) {
        return false;
    }

    auto device_ctx = reinterpret_cast<AVHWFramesContext*>(frame->hw_frames_ctx->data);
    auto d3d11va_ctx = reinterpret_cast<AVD3D11VADeviceContext*>(device_ctx->device_ctx->hwctx);
    Microsoft::WRL::ComPtr<ID3D11Query>query;
    D3D11_QUERY_DESC desc_query;
    desc_query.Query = D3D11_QUERY_EVENT;
    desc_query.MiscFlags = 0;
    if (ret = d3d11va_ctx->device->CreateQuery(&desc_query, query.GetAddressOf()); ret != S_OK || !query) {
        return false;
    }
    ret = d3d11va_ctx->device->OpenSharedResource(sharedHandle, __uuidof(ID3D11Texture2D), (void**)sharedTexture.GetAddressOf());
    if (ret != S_OK) {
        return false;
    }

    D3D11_TEXTURE2D_DESC dstRealDesc;
    sharedTexture->GetDesc(&dstRealDesc);

    d3d11va_ctx->lock(d3d11va_ctx->lock_ctx);
    D3D11_BOX box{ 0,0,0,dstRealDesc.Width, dstRealDesc.Height,1 };
    d3d11va_ctx->device_context->CopySubresourceRegion(sharedTexture.Get(), 0, 0, 0, 0, srcTexture, (UINT)frame->data[1], &box);
    d3d11va_ctx->device_context->End(query.Get());
    d3d11va_ctx->unlock(d3d11va_ctx->lock_ctx);
    while (S_FALSE == d3d11va_ctx->device_context->GetData(query.Get(), nullptr, 0, 0)) {

    }
    //d3d11va_ctx->unlock(d3d11va_ctx->lock_ctx);
    return true;
}

bool CopyD3D11Texture2D3D11Texture(Microsoft::WRL::ComPtr<ID3D11Texture2D>& dstTexture, ID3D11Device* dstDevice, const Microsoft::WRL::ComPtr<ID3D11Texture2D>& srcTexture) {
    ID3D10Multithread* pMultithread = nullptr;
    HRESULT ret = S_OK;
    ret = dstDevice->QueryInterface(__uuidof(ID3D10Multithread), reinterpret_cast<void**>(&pMultithread));
    if (ret == S_OK) {
        pMultithread->SetMultithreadProtected(TRUE);
        pMultithread->Release();
    }
    Microsoft::WRL::ComPtr<ID3D11Device> srcDevice;
    srcTexture->GetDevice(srcDevice.GetAddressOf());
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> srcDeviceCtx;
    srcDevice->GetImmediateContext(&srcDeviceCtx);

    Microsoft::WRL::ComPtr<ID3D11Query>query;
    D3D11_QUERY_DESC desc_query;
    desc_query.Query = D3D11_QUERY_EVENT;
    desc_query.MiscFlags = 0;
    if (ret = srcDevice->CreateQuery(&desc_query, query.GetAddressOf()); ret != S_OK || !query) {
        return false;
    }

    srcDeviceCtx->Begin(query.Get());
    srcDeviceCtx->CopySubresourceRegion(dstTexture.Get(), 0, 0, 0, 0, srcTexture.Get(), 0, 0);
    srcDeviceCtx->End(query.Get());

    while (S_FALSE == srcDeviceCtx->GetData(query.Get(), nullptr, 0, 0)) {

    }
    return true;
}