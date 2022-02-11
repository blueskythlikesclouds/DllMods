#pragma once

#include "Surface.h"

class RenderTargetSurface : public Surface
{
    ComPtr<RenderTargetTexture> texture;

public:
    explicit RenderTargetSurface(const ComPtr<Device>& device, const ComPtr<RenderTargetTexture>& texture);

    RenderTargetTexture* getTexture() const;
};
