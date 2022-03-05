#include "Texture.h"

#include "TypeConverter.h"

Texture::Texture(Device* device, ID3D11Resource* resource, DXGI_FORMAT format)
    : BaseTexture(device, resource), format(format)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};

    D3D11_RESOURCE_DIMENSION type;
    resource->GetType(&type);

    switch (type)
    {

    case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
    {
        ComPtr<ID3D11Texture2D> tex;
        resource->QueryInterface(tex.GetAddressOf());

        D3D11_TEXTURE2D_DESC desc;
        tex->GetDesc(&desc);

        srvDesc.Format = TypeConverter::makeColor(format);

        if (desc.ArraySize == 6)
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            srvDesc.TextureCube.MipLevels = desc.MipLevels;
        }
        else
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = desc.MipLevels;
        }

        break;
    }

    case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
    {
        ComPtr<ID3D11Texture3D> tex;
        resource->QueryInterface(tex.GetAddressOf());

        D3D11_TEXTURE3D_DESC desc;
        tex->GetDesc(&desc);

        format = desc.Format;

        srvDesc.Format = TypeConverter::makeColor(format);
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
        srvDesc.Texture3D.MipLevels = desc.MipLevels;

        break;
    }

    }

    device->get()->CreateShaderResourceView(resource, &srvDesc, srv.GetAddressOf());
}

Texture::Texture(Device* device, ID3D11Resource* resource, ID3D11ShaderResourceView* srv)
    : BaseTexture(device, resource), srv(srv)
{
}

DXGI_FORMAT Texture::getFormat() const
{
    return format;
}

ID3D11ShaderResourceView* Texture::getSRV() const
{
    return srv.Get();
}

HRESULT Texture::GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc)
{
    if (!pDesc)
        return E_INVALIDARG;

    ZeroMemory(pDesc, sizeof(*pDesc));

    D3D11_RESOURCE_DIMENSION type;
    resource->GetType(&type);

    switch (type)
    {

    case D3D11_DSV_DIMENSION_TEXTURE2D:
    {
        ComPtr<ID3D11Texture2D> tex;
        resource->QueryInterface(tex.GetAddressOf());

        D3D11_TEXTURE2D_DESC desc;
        tex->GetDesc(&desc);

        pDesc->Width = desc.Width >> Level;
        pDesc->Height = desc.Height >> Level;

        break;
    }

    case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
    {
        ComPtr<ID3D11Texture3D> tex;
        resource->QueryInterface(tex.GetAddressOf());

        D3D11_TEXTURE3D_DESC desc;
        tex->GetDesc(&desc);

        pDesc->Width = desc.Width >> Level;
        pDesc->Height = desc.Height >> Level;

        break;
    }

    }

    return S_OK;
}

FUNCTION_STUB(HRESULT, Texture::GetSurfaceLevel, UINT Level, Surface** ppSurfaceLevel)

HRESULT Texture::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)
{
    D3D11_MAPPED_SUBRESOURCE mappedSubResource;

    const auto lock = device->lock();

    if (FAILED(device->getContext()->Map(resource.Get(), Level, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
        return E_FAIL;

    pLockedRect->pBits = mappedSubResource.pData;
    pLockedRect->Pitch = (INT)mappedSubResource.RowPitch;

    return S_OK;
}

HRESULT Texture::UnlockRect(UINT Level)
{
    const auto lock = device->lock();

    device->getContext()->Unmap(resource.Get(), Level);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Texture::AddDirtyRect, CONST RECT* pDirtyRect)