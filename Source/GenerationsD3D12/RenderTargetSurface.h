#pragma once

#include "Surface.h"

class RenderTargetSurface : public Surface
{
    ComPtr<RenderTargetTexture> d3dRenderTargetTexture;

public:
    explicit RenderTargetSurface(const ComPtr<Device>& d3dDevice, const ComPtr<RenderTargetTexture>& d3dRenderTargetTexture);

    RenderTargetTexture* getRenderTargetTexture() const;
};
