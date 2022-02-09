#include "Surface.h"

Surface::Surface(const ComPtr<Device>& d3dDevice, const ComPtr<ID3D12Resource>& d3d12Resource) : Resource(d3dDevice, d3d12Resource)
{
}

FUNCTION_STUB(HRESULT, Surface::GetContainer, REFIID riid, void** ppContainer)

FUNCTION_STUB(HRESULT, Surface::GetDesc, D3DSURFACE_DESC *pDesc)

FUNCTION_STUB(HRESULT, Surface::LockRect, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)

FUNCTION_STUB(HRESULT, Surface::UnlockRect)

FUNCTION_STUB(HRESULT, Surface::GetDC, HDC *phdc)

FUNCTION_STUB(HRESULT, Surface::ReleaseDC, HDC hdc)

