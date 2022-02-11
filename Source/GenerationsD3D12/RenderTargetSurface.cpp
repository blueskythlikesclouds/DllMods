#include "RenderTargetSurface.h"
#include "RenderTargetTexture.h"

RenderTargetSurface::RenderTargetSurface(const ComPtr<Device>& d3dDevice, const ComPtr<RenderTargetTexture>& d3dRenderTargetTexture)
    : Surface(d3dDevice, d3dRenderTargetTexture->getD3DResource()), d3dRenderTargetTexture(d3dRenderTargetTexture)
{
}

RenderTargetTexture* RenderTargetSurface::getRenderTargetTexture() const
{
    return d3dRenderTargetTexture.Get();
}
