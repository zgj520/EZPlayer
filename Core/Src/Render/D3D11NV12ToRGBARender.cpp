#include "D3D11NV12ToRGBARender.h"
#include "d3d11.h"
#include "EGL\eglext.h"
#include <wrl/client.h>

D3D11NV12ToRGBARender::D3D11NV12ToRGBARender(long windid):VideoBaseRender(windid) {
}

void D3D11NV12ToRGBARender::render(AVFrame* frame) {
    ID3D11Texture2D* texture = reinterpret_cast<ID3D11Texture2D*>(frame->data[0]);
    D3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);
    if (desc.Width == 0 || desc.Height == 0) {
        return;
    }

    switch (desc.Format)
    {
    case DXGI_FORMAT_P016:
    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_NV12:

    default:
        return;
    } 
    D3D11_TEXTURE2D_DESC textureDesc = desc;
    textureDesc.ArraySize = 1;
    textureDesc.MipLevels = 1;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.Width = frame->width;
    textureDesc.Height = frame->height;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> d3d11Texture;
    auto hr = m_glD3d11Device->CreateTexture2D(&textureDesc, NULL, d3d11Texture.GetAddressOf());
    if (FAILED(hr)) {
        return;
    }

    GLuint textures[2];
    glGenTextures(2, textures);
    for (auto textureId: textures) {
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureId);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    auto client_buf = static_cast<EGLClientBuffer>(d3d11Texture.Get());
    EGLint attribs[] = {EGL_D3D11_TEXTURE_PLANE_ANGLE, 0, EGL_NONE };
    //EGLImage yImage = eglCreateImageKHR(m_display, EGL_NO_CONTEXT, EGL_D3D11_TEXTURE_ANGLE, client_buf, attribs);
}
