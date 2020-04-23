#include "D3D9ExSurface.h"

D3D9ExSurface::D3D9ExSurface(D3D9ExDevice* d3dDevice, IDirect3DSurface9* d3dSurface) : D3D9ExResource(d3dDevice, d3dSurface), d3dSurface(d3dSurface)
{
}

HRESULT D3D9ExSurface::GetContainer(REFIID riid, void** ppContainer)
{
    return d3dSurface->GetContainer(riid, ppContainer);
}

HRESULT D3D9ExSurface::GetDesc(D3DSURFACE_DESC* pDesc)
{
    return d3dSurface->GetDesc(pDesc);
}

HRESULT D3D9ExSurface::LockRect(D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)
{
    return d3dSurface->LockRect(pLockedRect, pRect, Flags);
}

HRESULT D3D9ExSurface::UnlockRect()
{
    return d3dSurface->UnlockRect();
}

HRESULT D3D9ExSurface::GetDC(HDC* phdc)
{
    return d3dSurface->GetDC(phdc);
}

HRESULT D3D9ExSurface::ReleaseDC(HDC hdc)
{
    return d3dSurface->ReleaseDC(hdc);
}