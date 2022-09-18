#pragma once
#include "VideoBaseRender.h"
#include <string>

class BMPImageRender: public VideoBaseRender
{
public:
    BMPImageRender(long winid, const std::string& filePath);
    ~BMPImageRender() {};

    void render();

private:
    GLubyte* parseBmpImageData(int& width, int& height);

    void renderImage(void* imageData, int width, int height);

private:
    std::string m_filePath;
};