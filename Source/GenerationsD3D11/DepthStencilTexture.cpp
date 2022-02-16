#include "DepthStencilTexture.h"

#include "DepthStencilSurface.h"

DepthStencilTexture::DepthStencilTexture(Device* device, ID3D11Resource* resource, DXGI_FORMAT format)
    : Texture(device, resource, format)
{
    D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
    desc.Format = format;
    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    device->get()->CreateDepthStencilView(resource, format != DXGI_FORMAT_UNKNOWN ? &desc : nullptr, dsv.GetAddressOf());
}

ID3D11DepthStencilView* DepthStencilTexture::getDSV() const
{
    return dsv.Get();
}

HRESULT DepthStencilTexture::GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel)
{
    if (!surface)
        surface = new DepthStencilSurface(device.Get(), this);

    *ppSurfaceLevel = surface.Get();
    (*ppSurfaceLevel)->AddRef();
    return S_OK;
}
