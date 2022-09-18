#pragma once
#include "VideoBaseRender.h"

class D3D11NV12ToRGBARender: public VideoBaseRender
{
public:
    D3D11NV12ToRGBARender(long winid);
    ~D3D11NV12ToRGBARender() {};

    void render(AVFrame* frame) override;
};