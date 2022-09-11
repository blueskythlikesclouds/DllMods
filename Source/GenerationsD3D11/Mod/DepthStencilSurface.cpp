#include "DepthStencilSurface.h"

#include "DepthStencilTexture.h"

DepthStencilSurface::DepthStencilSurface(Device* device, DepthStencilTexture* texture, const size_t mipSlice)
    : Surface(device, texture->getResource())
{
    D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
    desc.Format = texture->getFormat();
    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipSlice = mipSlice;

    device->get()->CreateDepthStencilView(texture->getResource(), &desc, dsv.GetAddressOf());
}

ID3D11DepthStencilView* DepthStencilSurface::getDSV() const
{
    return dsv.Get();
}
