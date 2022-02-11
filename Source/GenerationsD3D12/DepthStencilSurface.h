#pragma once

#include "Surface.h"

class DepthStencilSurface : public Surface
{
    ComPtr<DepthStencilTexture> texture;

public:
    explicit DepthStencilSurface(const ComPtr<Device>& device, const ComPtr<DepthStencilTexture>& texture);

    DepthStencilTexture* getTexture() const;
};
