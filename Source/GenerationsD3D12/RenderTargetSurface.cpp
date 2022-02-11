#include "RenderTargetSurface.h"
#include "RenderTargetTexture.h"

RenderTargetSurface::RenderTargetSurface(const ComPtr<Device>& device, const ComPtr<RenderTargetTexture>& texture)
    : Surface(device, texture->getResource()), texture(texture)
{
}

RenderTargetTexture* RenderTargetSurface::getTexture() const
{
    return texture.Get();
}
