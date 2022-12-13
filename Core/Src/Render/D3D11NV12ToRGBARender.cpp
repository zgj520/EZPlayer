#include "D3D11NV12ToRGBARender.h"
#include "d3d11.h"
#include "EGL\eglext.h"
#include <wrl/client.h>

D3D11NV12ToRGBARender::D3D11NV12ToRGBARender(long windid):VideoBaseRender(windid, AV_PIX_FMT_D3D11) {
    kVS = R"(
attribute vec4 vPosition;
attribute vec2 aSamplerCoord;
varying vec2 vSamplerCoord;
void main()
{
    vSamplerCoord = aSamplerCoord;
    gl_Position = vPosition;
})";

    kFS = R"(
precision mediump float;
varying vec2 vSamplerCoord;
uniform sampler2D uSamplerTexture1;
uniform sampler2D uSamplerTexture2;
void main()
{
    vec3 yuv;
    yuv.x = texture2D(uSamplerTexture1, vSamplerCoord).r - 0.0625;
    yuv.y = texture2D(uSamplerTexture2, vSamplerCoord).r - 0.5;
    yuv.z = texture2D(uSamplerTexture2, vSamplerCoord).g - 0.5;
    mat3 nv12_to_rgba = mat3( 1,       1,        1,
                              0,       -0.39465, 2.03211,
                              1.13983, -0.58060, 0);
    gl_FragColor = vec4(nv12_to_rgba * yuv, 1.0);
})";
}

GLuint D3D11NV12ToRGBARender::genTexture(int width, int height, GLuint format) {
    GLuint texture_ID = 0;
    glGenTextures(1, &texture_ID);
    GLint lastTextureID = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
    glBindTexture(GL_TEXTURE_2D, texture_ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, lastTextureID);
    return texture_ID;
}

void D3D11NV12ToRGBARender::render(AVFrame* frame) {
    ID3D11Texture2D* texture = reinterpret_cast<ID3D11Texture2D*>(frame->data[0]);
    D3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);
    if (desc.Width == 0 || desc.Height == 0) {
        return;
    }
    Microsoft::WRL::ComPtr<ID3D11Device> decodeDevice;
    texture->GetDevice(decodeDevice.GetAddressOf());
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> decodeDeviceCtx;
    decodeDevice->GetImmediateContext(&decodeDeviceCtx);

    switch (desc.Format)
    {
    case DXGI_FORMAT_P016:
    case DXGI_FORMAT_P010:
        return;
    case DXGI_FORMAT_NV12:
        break;
    default:
        return;
    } 
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

    Microsoft::WRL::ComPtr<ID3D11Texture2D> d3d11Texture;
    auto hr = m_glD3d11Device->CreateTexture2D(&tdesc, NULL, d3d11Texture.GetAddressOf());
    if (FAILED(hr)) {
        return;
    }
    HANDLE sharedHandle;
    Microsoft::WRL::ComPtr<IDXGIResource> dxgiShareTexture;
    d3d11Texture->QueryInterface(__uuidof(IDXGIResource), (void**)dxgiShareTexture.GetAddressOf());
    hr = dxgiShareTexture->GetSharedHandle(&sharedHandle);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> videoTexture;
    hr = decodeDevice->OpenSharedResource(sharedHandle, __uuidof(ID3D11Texture2D), (void**)&videoTexture);
    decodeDeviceCtx->CopySubresourceRegion(videoTexture.Get(), 0, 0, 0, 0, texture, (int)frame->data[1], 0);
    decodeDeviceCtx->Flush();

    if (m_programId == 0) {
        m_programId = glCreateProgram();
        if (!compileSharder(&m_vertShader, GL_VERTEX_SHADER, kVS.c_str()) || !compileSharder(&m_fragShader, GL_FRAGMENT_SHADER, kFS.c_str())) {
            m_programId = 0;
            return;
        }
        glAttachShader(m_programId, m_vertShader);
        glAttachShader(m_programId, m_fragShader);
        glLinkProgram(m_programId);
    }

    int width = frame->width;
    int height = frame->height;

    RECT RC;
    ::GetClientRect((HWND)m_windid, &RC);
    // Set the viewport
    glViewport(0, 0, RC.right - RC.left, RC.bottom - RC.top);

    // Use the program object
    glUseProgram(m_programId);

    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //定义定点数组
    float vertex_coord_data[] = {
            -1.f, -1.f, 0.f,   0.f, 1.f,
            -1.f,  1.f, 0.f,   0.f, 0.f,
             1.f,  1.f, 0.f,   1.f, 0.f,
             1.f, -1.f, 0.f,   1.f, 1.f,
    };

    uint32_t vertx_index_data[] = {
        0, 1, 2,
        2, 3, 0
    };

    uint32_t m_vertex_buffer, m_index_buffer;
    glGenBuffers(1, &m_vertex_buffer);
    glGenBuffers(1, &m_index_buffer);

    GLuint m_vertex_array;
    glGenVertexArrays(1, &m_vertex_array);
    glBindVertexArray(m_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_coord_data), vertex_coord_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertx_index_data), vertx_index_data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    auto client_buf = static_cast<EGLClientBuffer>(d3d11Texture.Get());
    EGLint attribs[] = {EGL_D3D11_TEXTURE_PLANE_ANGLE, 0, EGL_NONE };
    EGLImage yImage = eglCreateImageKHR(m_display, EGL_NO_CONTEXT, EGL_D3D11_TEXTURE_ANGLE, client_buf, attribs);
    if (m_yTextureId == 0) {
        glGenTextures(1, &m_yTextureId);
    }
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, m_yTextureId);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, yImage);
    auto error = glGetError();
    if (error != GL_NO_ERROR) {
        return;
    }
    glUniform1i(glGetUniformLocation(m_programId, "uSamplerTexture1"), 0);

    attribs[1] = 1;
    EGLImage uvImage = eglCreateImageKHR(m_display, EGL_NO_CONTEXT, EGL_D3D11_TEXTURE_ANGLE, client_buf, attribs);
    if (m_uvTextureId == 0) {
        glGenTextures(1, &m_uvTextureId);
    }
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, m_uvTextureId);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, uvImage);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        return;
    }
    glUniform1i(glGetUniformLocation(m_programId, "uSamplerTexture2"), 1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, vertx_index_data);

    eglSwapBuffers(m_display, m_surface);

    glDeleteVertexArrays(1, &m_vertex_array);
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteBuffers(1, &m_index_buffer);

}
