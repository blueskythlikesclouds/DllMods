#pragma once

#include "Surface.h"

class RenderTargetTexture;

class RenderTargetSurface : public Surface
{
    ComPtr<ID3D11RenderTargetView> rtv;

public:
    explicit RenderTargetSurface(Device* device, RenderTargetTexture* texture, size_t mipSlice);

    ID3D11RenderTargetView* getRTV() const;
};
