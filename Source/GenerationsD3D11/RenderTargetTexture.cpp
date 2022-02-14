#include "RenderTargetTexture.h"

#include "RenderTargetSurface.h"

RenderTargetTexture::RenderTargetTexture(Device* device, ID3D11Resource* resource, DXGI_FORMAT format)
    : Texture(device, resource, format)
{
    D3D11_RENDER_TARGET_VIEW_DESC desc{};
    desc.Format = format;
    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    device->getDevice()->CreateRenderTargetView(resource, format != DXGI_FORMAT_UNKNOWN ? &desc : nullptr, rtv.GetAddressOf());
}

ID3D11RenderTargetView* RenderTargetTexture::getRTV() const
{
    return rtv.Get();
}

HRESULT RenderTargetTexture::GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel)
{
    if (!surface)
        surface = new RenderTargetSurface(device.Get(), this);

    *ppSurfaceLevel = surface.Get();
    (*ppSurfaceLevel)->AddRef();
    return S_OK;
}
