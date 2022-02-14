#include "Surface.h"

Surface::Surface(Device* device, ID3D11Resource* resource) : Resource(device, resource)
{
}

FUNCTION_STUB(HRESULT, Surface::GetContainer, REFIID riid, void** ppContainer)

HRESULT Surface::GetDesc(D3DSURFACE_DESC *pDesc)
{
    ComPtr<ID3D11Texture2D> texture;
    if (resource.As(&texture))
        return E_FAIL;

    D3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);

    ZeroMemory(pDesc, sizeof(*pDesc));
    pDesc->Width = desc.Width;
    pDesc->Height = desc.Height;

    return S_OK;
}

FUNCTION_STUB(HRESULT, Surface::LockRect, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)

FUNCTION_STUB(HRESULT, Surface::UnlockRect)

FUNCTION_STUB(HRESULT, Surface::GetDC, HDC *phdc)

FUNCTION_STUB(HRESULT, Surface::ReleaseDC, HDC hdc)

