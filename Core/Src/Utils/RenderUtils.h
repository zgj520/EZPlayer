#pragma once
#include <Windows.h>
#include <wrl\client.h>
#include <d3d11.h>

class AVFrame;
bool CopyD3D11AVFrame2D3D11Texture(Microsoft::WRL::ComPtr<ID3D11Texture2D>& dstTexture, ID3D11Device* dstDevice,AVFrame* frame);

bool CopyD3D11Texture2D3D11Texture(Microsoft::WRL::ComPtr<ID3D11Texture2D>& dstTexture, ID3D11Device* dstDevice, const Microsoft::WRL::ComPtr<ID3D11Texture2D>& srcTexture);