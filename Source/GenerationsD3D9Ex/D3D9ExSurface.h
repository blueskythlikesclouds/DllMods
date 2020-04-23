#pragma once

#include "D3D9ExResource.h"

class D3D9ExSurface : public D3D9ExResource
{
    IDirect3DSurface9* d3dSurface;

public:
    D3D9ExSurface(D3D9ExDevice* d3dDevice, IDirect3DSurface9* d3dSurface);

    virtual HRESULT GetContainer(const IID& riid, void** ppContainer);
    virtual HRESULT GetDesc(D3DSURFACE_DESC* pDesc);
    virtual HRESULT LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
    virtual HRESULT UnlockRect();
    virtual HRESULT GetDC(HDC* phdc);
    virtual HRESULT ReleaseDC(HDC hdc);

    IDirect3DSurface9* GetD3DSurface() const
    {
        return d3dSurface;
    }
};
