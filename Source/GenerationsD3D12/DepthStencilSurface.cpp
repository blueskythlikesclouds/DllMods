#include "DepthStencilSurface.h"
#include "DepthStencilTexture.h"

DepthStencilSurface::DepthStencilSurface(const ComPtr<Device>& device, const ComPtr<DepthStencilTexture>& texture)
    : Surface(device, texture->getResource()), texture(texture)
{
}

DepthStencilTexture* DepthStencilSurface::getTexture() const
{
    return texture.Get();
}
