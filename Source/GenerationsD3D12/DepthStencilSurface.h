#pragma once

#include "Surface.h"

class DepthStencilSurface : public Surface
{
    ComPtr<DepthStencilTexture> d3dDepthStencilTexture;

public:
    explicit DepthStencilSurface(const ComPtr<Device>& d3dDevice, const ComPtr<DepthStencilTexture>& d3dDepthStencilTexture);

    DepthStencilTexture* getDepthStencilTexture() const;
};
