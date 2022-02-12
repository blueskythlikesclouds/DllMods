#include "DepthStencilSurface.h"

#include "DepthStencilTexture.h"

DepthStencilSurface::DepthStencilSurface(Device* device, DepthStencilTexture* texture)
    : Surface(device, texture->getResource()), texture(texture)
{
}

DepthStencilTexture* DepthStencilSurface::getTexture() const
{
    return texture.Get();
}
