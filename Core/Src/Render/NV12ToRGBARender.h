#pragma once
#include "VideoBaseRender.h"

class NV12ToRGBARender: public VideoBaseRender
{
public:
    NV12ToRGBARender(long winid);
    ~NV12ToRGBARender();

    void render(AVFrame* frame) override;

private:
    GLuint genTexture(int width, int height, GLuint format = GL_RED);

private:
    GLuint m_yTextureId = 0;
    GLuint m_uvTextureId = 0;
};