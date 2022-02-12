#pragma once

#include "Surface.h"

class RenderTargetSurface : public Surface
{
    ComPtr<RenderTargetTexture> texture;

public:
    explicit RenderTargetSurface(Device* device, RenderTargetTexture* texture);

    RenderTargetTexture* getTexture() const;
};
