#include "RenderTargetSurface.h"
#include "RenderTargetTexture.h"

RenderTargetSurface::RenderTargetSurface(Device* device, RenderTargetTexture* texture)
    : Surface(device, texture->getResource()), texture(texture)
{
}

RenderTargetTexture* RenderTargetSurface::getTexture() const
{
    return texture.Get();
}
