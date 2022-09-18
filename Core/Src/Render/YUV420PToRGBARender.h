#pragma once
#include "VideoBaseRender.h"

class YUV420PToRGBARender: public VideoBaseRender
{
public:
    YUV420PToRGBARender(long winid);
    ~YUV420PToRGBARender() {};

    void render(AVFrame* frame) override;

private:
    GLuint genTexture(int width, int height);

private:
    GLuint m_yTextureId = 0;
    GLuint m_uTextureId = 0;
    GLuint m_vTextureId = 0;
};