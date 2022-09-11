#pragma once

#include "Surface.h"

class DepthStencilTexture;

class DepthStencilSurface : public Surface
{
    ComPtr<ID3D11DepthStencilView> dsv;

public:
    explicit DepthStencilSurface(Device* device, DepthStencilTexture* texture, size_t mipSlice);

    ID3D11DepthStencilView* getDSV() const;
};
