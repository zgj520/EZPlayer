#pragma once
#include <Windows.h>
#include <wrl\client.h>
#include <d3d11.h>
#include "VideoBaseRender.h"

enum RENDER_TYPE
{
    RENDER_RECTANGLE_DEMO,
    RENDER_NV12TORGBA
};

enum SHADER_TYPE
{
    SHADER_VERTEX, //顶点着色器
    SHADER_PIXEL // 像素着色器
};

class D3D11DXRender : public VideoBaseRender {
public:
    D3D11DXRender(HWND hwnd);

    void renderDemo();

    void render(AVFrame* frame) override;

    bool init() override;

private:
    bool compileShader(RENDER_TYPE type);

    const BYTE* getShader(RENDER_TYPE renderType, SHADER_TYPE shaderType, uint64_t& size);

    void updateSwapChainBufferSize();

private:
    HWND m_hwnd = NULL;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_spSwapChain = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Device> m_spD3DDeivce = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_spD3DDeviceCtx = nullptr;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_spVertexShader = nullptr;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_spPixelShader = nullptr;
};