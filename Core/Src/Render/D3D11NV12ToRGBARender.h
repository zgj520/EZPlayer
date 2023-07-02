#pragma once
#include "VideoBaseRender.h"

class D3D11NV12ToRGBARender: public VideoBaseRender
{
public:
    D3D11NV12ToRGBARender(long winid);
    ~D3D11NV12ToRGBARender() {};

    void render(AVFrame* frame) override;

private:
    GLuint genTexture(int width, int height, GLuint format = GL_RED);

private:
    GLuint m_yTextureId = 0;
    GLuint m_uvTextureId = 0;
};