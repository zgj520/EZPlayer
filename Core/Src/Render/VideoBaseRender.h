#pragma once
extern "C" {
#include "libavformat/avformat.h"
}
#include "angle_gl.h"
#include "EGL\egl.h"
#include <Windows.h>
#include <d3d11.h>
#include <d3d9.h>
#include <string>

class VideoBaseRender
{
public:
    VideoBaseRender(long winid);
    ~VideoBaseRender() {};

    virtual void render();

    virtual void render(AVFrame*) {};

    virtual bool init();

    virtual bool compileSharder(GLuint* shader, GLenum shaderType, const char* source);

protected:
    GLint createTextureFromImgaeData(void* imageData, int width, int height);

private:
    virtual void destroyGL();
    virtual void destroySurface();
    virtual void destroyContext();

protected:
    long m_windid = 0;
    EGLDisplay m_display = EGL_NO_DISPLAY;
    EGLSurface m_surface = EGL_NO_SURFACE;
    EGLContext m_context = EGL_NO_CONTEXT;
    EGLConfig  m_config = nullptr;
    std::string kVS;
    std::string kFS;
    GLuint m_vertShader = 0;
    GLuint m_fragShader = 0;
    GLuint m_programId = 0;

    ID3D11Device* m_glD3d11Device = nullptr;
    IDirect3DDevice9* m_glD3d9Device = nullptr;
};