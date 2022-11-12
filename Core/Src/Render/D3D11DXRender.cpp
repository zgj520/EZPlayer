#include "D3D11DXRender.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "TextureVertexShader.h"
#include "TexturePixelShader.h"

#pragma comment(lib, "d3d11.lib")

struct Vertex {
    float x; float y; float z;
};
struct Vertex2 {
    float x; float y; float z;
    struct
    {
        float u;
        float v;
    } tex;
};



D3D11DXRender::D3D11DXRender(HWND hwnd) :VideoBaseRender((long)hwnd, AV_PIX_FMT_D3D11) ,m_hwnd(hwnd) {

}

void D3D11DXRender::renderDemo() {
    compileShader(RENDER_RECTANGLE_DEMO);

    RECT rc;
    ::GetClientRect(m_hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    // 防止窗口变化就行更新
    updateSwapChainBufferSize();

    // 顶点输入
    const Vertex vertices[] = {
        {-1,  1,  0},
        { 1,  1,  0},
        { 1, -1,  0},
        {-0.5, -0.5,  0},
    };
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(vertices);
    bd.StructureByteStride = sizeof(Vertex);
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
    m_spD3DDeivce->CreateBuffer(&bd, &sd, &pVertexBuffer);
    UINT stride = sizeof(Vertex);
    UINT offset = 0u;
    ID3D11Buffer* vertexBuffers[] = { pVertexBuffer.Get() };
    m_spD3DDeviceCtx->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

    //  顶点索引
    const UINT16 indices[] = {
        0,1,2, 0,2,3
    };
    auto indicesSize = 6;
    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.ByteWidth = sizeof(indices);
    ibd.StructureByteStride = sizeof(UINT16);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
    m_spD3DDeivce->CreateBuffer(&ibd, &isd, &pIndexBuffer);
    m_spD3DDeviceCtx->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    m_spD3DDeviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 光栅化
    D3D11_VIEWPORT viewPort = {};
    viewPort.TopLeftX = 0;
    viewPort.TopLeftY = 0;
    viewPort.Width = rc.right - rc.left;
    viewPort.Height = rc.bottom - rc.top;
    viewPort.MaxDepth = 1;
    viewPort.MinDepth = 0;
    m_spD3DDeviceCtx->RSSetViewports(1, &viewPort);

    // 写入缓冲
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_spSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8B8A8_UNORM);
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  rtv;
    m_spD3DDeivce->CreateRenderTargetView(backBuffer.Get(), &renderTargetViewDesc, &rtv);
    ID3D11RenderTargetView* rtvs[] = { rtv.Get() };
    m_spD3DDeviceCtx->OMSetRenderTargets(1, rtvs, nullptr);

    // Draw Call
    m_spD3DDeviceCtx->DrawIndexed(indicesSize, 0, 0);
    m_spSwapChain->Present(1, 0);
}

void D3D11DXRender::render(AVFrame* frame) {
    RECT rc;
    ::GetClientRect(m_hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    updateSwapChainBufferSize();

    compileShader(RENDER_NV12TORGBA);

    HRESULT ret = S_FALSE;
    // 顶点输入
    const Vertex2 vertices[] = {
        {-1,	1,	0,	0,	0},
        {1,   	1,	0,	1,	0},
        {1, 	-1,	0,	1,	1},
        {-1,	-1,	0,	0,	1},
    };
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(vertices);
    bd.StructureByteStride = sizeof(Vertex2);
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
    ret = m_spD3DDeivce->CreateBuffer(&bd, &sd, &pVertexBuffer);
    ID3D11Buffer* vertexBuffers[] = {pVertexBuffer.Get() };
    UINT stride = sizeof(Vertex2);
    UINT offset = 0u;
    m_spD3DDeviceCtx->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

    const UINT16 indices[] = {0,1,2, 0,2,3};
    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.ByteWidth = sizeof(indices);
    ibd.StructureByteStride = sizeof(UINT16);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
    ret = m_spD3DDeivce->CreateBuffer(&ibd, &isd, &pIndexBuffer);
    m_spD3DDeviceCtx->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    m_spD3DDeviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 传递顶点参数
    D3D11_INPUT_ELEMENT_DESC ied[] = {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
    uint64_t shaderSize = 0;
    auto vShader = getShader(RENDER_NV12TORGBA, SHADER_VERTEX, shaderSize);
    ret = m_spD3DDeivce->CreateInputLayout(ied, std::size(ied), vShader, shaderSize, &pInputLayout);
    m_spD3DDeviceCtx->IASetInputLayout(pInputLayout.Get());

    // 创建采样器
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MaxAnisotropy = 16;
    Microsoft::WRL::ComPtr <ID3D11SamplerState>pSampler;
    ret = m_spD3DDeivce->CreateSamplerState(&samplerDesc, &pSampler);
    ID3D11SamplerState* samplers[] = { pSampler.Get() };
    m_spD3DDeviceCtx->PSSetSamplers(0, 1, samplers);

    // 从解码设备拷贝解码帧到渲染设备
    ID3D11Texture2D* t_frame = (ID3D11Texture2D*)frame->data[0];
    if (t_frame == nullptr) {
        return;
    }
    Microsoft::WRL::ComPtr<ID3D11Device> decodeDevice;
    t_frame->GetDevice(decodeDevice.GetAddressOf());
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> decodeDeviceCtx;
    decodeDevice->GetImmediateContext(&decodeDeviceCtx);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> dstTexture;
    D3D11_TEXTURE2D_DESC tdesc = {};
    tdesc.Format = DXGI_FORMAT_NV12;
    tdesc.Usage = D3D11_USAGE_DEFAULT;
    tdesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
    tdesc.ArraySize = 1;
    tdesc.MipLevels = 1;
    tdesc.SampleDesc = { 1, 0 };
    tdesc.Height = frame->height;
    tdesc.Width = frame->width;
    tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    ret = m_spD3DDeivce->CreateTexture2D(&tdesc, nullptr, &dstTexture);
    HANDLE sharedHandle;
    Microsoft::WRL::ComPtr<IDXGIResource> dxgiShareTexture;
    dstTexture->QueryInterface(__uuidof(IDXGIResource), (void**)dxgiShareTexture.GetAddressOf());
    ret = dxgiShareTexture->GetSharedHandle(&sharedHandle);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> videoTexture;
    ret = decodeDevice->OpenSharedResource(sharedHandle, __uuidof(ID3D11Texture2D), (void**)&videoTexture);
    decodeDeviceCtx->CopySubresourceRegion(videoTexture.Get(), 0, 0, 0, 0, t_frame, (int)frame->data[1], 0);
    decodeDeviceCtx->Flush();

    // 视频帧数据传递
    D3D11_SHADER_RESOURCE_VIEW_DESC const YPlaneDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC(dstTexture.Get(), D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8_UNORM );
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> y;
    m_spD3DDeivce->CreateShaderResourceView(dstTexture.Get(), &YPlaneDesc, &y);
    D3D11_SHADER_RESOURCE_VIEW_DESC const UVPlaneDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC( dstTexture.Get(), D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8_UNORM);
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> uv;
    m_spD3DDeivce->CreateShaderResourceView(dstTexture.Get(), &UVPlaneDesc, &uv);
    ID3D11ShaderResourceView* srvs[] = { y.Get(), uv.Get() };
    m_spD3DDeviceCtx->PSSetShaderResources(0, std::size(srvs), srvs);

    // 设置视窗
    D3D11_VIEWPORT viewPort = {};
    viewPort.TopLeftX = 0;
    viewPort.TopLeftY = 0;
    viewPort.Width = width;
    viewPort.Height = height;
    viewPort.MaxDepth = 1;
    viewPort.MinDepth = 0;
    m_spD3DDeviceCtx->RSSetViewports(1, &viewPort);

    // 创建显示缓存
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_spSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8B8A8_UNORM);
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  rtv;
    m_spD3DDeivce->CreateRenderTargetView(backBuffer.Get(), &renderTargetViewDesc, &rtv);
    ID3D11RenderTargetView* rtvs[] = { rtv.Get() };
    m_spD3DDeviceCtx->OMSetRenderTargets(1, rtvs, nullptr);

    // Draw Call
    auto indicesSize = std::size(indices);
    m_spD3DDeviceCtx->DrawIndexed(indicesSize, 0, 0);

    // 将缓存交换上屏
    m_spSwapChain->Present(1, 0);
}
bool D3D11DXRender::init() {
    RECT rc;
    ::GetClientRect(m_hwnd, &rc);

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    auto& bufferDesc = swapChainDesc.BufferDesc;
    bufferDesc.Width =  rc.right - rc.left;
    bufferDesc.Height = rc.bottom - rc.top;
    bufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.RefreshRate.Numerator = 0;
    bufferDesc.RefreshRate.Denominator = 0;
    bufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.OutputWindow = m_hwnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.Flags = 0;

    UINT flags = 0;
#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    auto ret = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, NULL, NULL, D3D11_SDK_VERSION,
        &swapChainDesc, &m_spSwapChain, &m_spD3DDeivce, NULL, &m_spD3DDeviceCtx);
    if (ret != S_OK) {
        return false;
    }
    return true;
}

bool D3D11DXRender::compileShader(RENDER_TYPE type) {
    uint64_t size = 0;
    HRESULT ret = S_FALSE;
    // 顶点着色器
    if (m_spVertexShader == nullptr) {
        auto vShader = getShader(type, SHADER_VERTEX, size);
        ret = m_spD3DDeivce->CreateVertexShader(vShader,  size, nullptr, &m_spVertexShader);
        m_spD3DDeviceCtx->VSSetShader(m_spVertexShader.Get(), 0, 0);
    }

    // 像素着色器
    if (m_spPixelShader == nullptr) {
        auto pixelShader = getShader(type, SHADER_PIXEL, size);
        ret = m_spD3DDeivce->CreatePixelShader(pixelShader, size, nullptr, &m_spPixelShader);
        m_spD3DDeviceCtx->PSSetShader(m_spPixelShader.Get(), 0, 0);
    }
    return true;
}

const BYTE* D3D11DXRender::getShader(RENDER_TYPE renderType, SHADER_TYPE shaderType, uint64_t& size) {
    const BYTE* pShader = nullptr;
    switch (renderType)
    {
    case RENDER_RECTANGLE_DEMO:
        if (shaderType == SHADER_VERTEX) {
            pShader = g_main_VS;
            size = sizeof(g_main_VS);
        }
        else {
            pShader = g_main_PS;
            size = sizeof(g_main_PS);
        }
    case RENDER_NV12TORGBA:
        if (shaderType == SHADER_VERTEX) {
            pShader = g_main_T_VS;
            size = sizeof(g_main_T_VS);
        }
        else {
            pShader = g_main_T_PS;
            size = sizeof(g_main_T_PS);
        }
    default:
        break;
    }
    return pShader;
}

void D3D11DXRender::updateSwapChainBufferSize() {
    RECT rc;
    ::GetClientRect(m_hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    DXGI_SWAP_CHAIN_DESC desc;
    m_spSwapChain->GetDesc(&desc);
    if (desc.BufferDesc.Width != width || desc.BufferDesc.Height != height) {
        m_spSwapChain->ResizeBuffers(desc.BufferCount, width, height, desc.BufferDesc.Format, desc.Flags);
    }
}