#include "DepthStencilSurface.h"
#include "DepthStencilTexture.h"

DepthStencilSurface::DepthStencilSurface(const ComPtr<Device>& d3dDevice, const ComPtr<DepthStencilTexture>& d3dDepthStencilTexture)
    : Surface(d3dDevice, d3dDepthStencilTexture->getD3DResource()), d3dDepthStencilTexture(d3dDepthStencilTexture)
{
}

DepthStencilTexture* DepthStencilSurface::getDepthStencilTexture() const
{
    return d3dDepthStencilTexture.Get();
}
