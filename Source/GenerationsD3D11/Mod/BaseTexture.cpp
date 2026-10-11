#include "BaseTexture.h"

BaseTexture::BaseTexture(Device* device, ID3D11Resource* resource) : Resource(device, resource)
{
}

ID3D11ShaderResourceView* BaseTexture::getSRV() const
{
    return srv.Get();
}

bool BaseTexture::getIsShadow() const
{
    return isShadow;
}

void BaseTexture::setIsShadow(bool value)
{
    isShadow = value;
}

FUNCTION_STUB(DWORD, BaseTexture::SetLOD, DWORD LODNew)

FUNCTION_STUB(DWORD, BaseTexture::GetLOD)

DWORD BaseTexture::GetLevelCount()
{
    ComPtr<ID3D11Texture2D> tex;
    if (resource && SUCCEEDED(resource.As(&tex)))
    {
        D3D11_TEXTURE2D_DESC desc;
        tex->GetDesc(&desc);
        return desc.MipLevels;
    }

    ComPtr<ID3D11Texture3D> volume;
    if (resource && SUCCEEDED(resource.As(&volume)))
    {
        D3D11_TEXTURE3D_DESC desc;
        volume->GetDesc(&desc);
        return desc.MipLevels;
    }

    return 1;
}

FUNCTION_STUB(HRESULT, BaseTexture::SetAutoGenFilterType, D3DTEXTUREFILTERTYPE FilterType)

FUNCTION_STUB(D3DTEXTUREFILTERTYPE, BaseTexture::GetAutoGenFilterType)

FUNCTION_STUB(void, BaseTexture::GenerateMipSubLevels)

FUNCTION_STUB(void, BaseTexture::FUN_48)

FUNCTION_STUB(HRESULT, BaseTexture::FUN_4C, void* A1)
