#include "Surface.h"

Surface::Surface(const ComPtr<Device>& device, const ComPtr<ID3D12Resource>& resource) : Resource(device, resource)
{
}

FUNCTION_STUB(HRESULT, Surface::GetContainer, REFIID riid, void** ppContainer)

HRESULT Surface::GetDesc(D3DSURFACE_DESC *pDesc)
{
    const D3D12_RESOURCE_DESC desc = resource->GetDesc();
    if (desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D)
        return E_FAIL;

    ZeroMemory(pDesc, sizeof(*pDesc));
    pDesc->Width = (UINT)desc.Width;
    pDesc->Height = (UINT)desc.Height;

    return S_OK;
}

FUNCTION_STUB(HRESULT, Surface::LockRect, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)

FUNCTION_STUB(HRESULT, Surface::UnlockRect)

FUNCTION_STUB(HRESULT, Surface::GetDC, HDC *phdc)

FUNCTION_STUB(HRESULT, Surface::ReleaseDC, HDC hdc)

