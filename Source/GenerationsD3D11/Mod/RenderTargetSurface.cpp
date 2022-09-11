#include "RenderTargetSurface.h"

#include "RenderTargetTexture.h"

RenderTargetSurface::RenderTargetSurface(Device* device, RenderTargetTexture* texture, const size_t mipSlice)
    : Surface(device, texture->getResource())
{
    D3D11_RENDER_TARGET_VIEW_DESC desc{};
    desc.Format = texture->getFormat();
    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipSlice = mipSlice;

    device->get()->CreateRenderTargetView(texture->getResource(), &desc, rtv.GetAddressOf());
}

ID3D11RenderTargetView* RenderTargetSurface::getRTV() const
{
    return rtv.Get();
}