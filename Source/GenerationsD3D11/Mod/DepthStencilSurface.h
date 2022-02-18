#pragma once

#include "Surface.h"

class DepthStencilSurface : public Surface
{
    ComPtr<DepthStencilTexture> texture;

public:
    explicit DepthStencilSurface(Device* device, DepthStencilTexture* texture);

    DepthStencilTexture* getTexture() const;
};
