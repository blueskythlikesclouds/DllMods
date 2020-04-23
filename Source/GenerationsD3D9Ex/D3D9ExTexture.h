#pragma once

#include "D3D9ExBaseTexture.h"

class D3D9ExSurface;

class D3D9ExTexture : public D3D9ExBaseTexture
{
    IDirect3DTexture9* d3dTexture;
    std::vector<D3D9ExSurface*> dxpSurfaces;

public:
    D3D9ExTexture(D3D9ExDevice* d3dDevice, IDirect3DTexture9* d3dTexture);
    ~D3D9ExTexture();

    virtual HRESULT GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc);
    virtual HRESULT GetSurfaceLevel(UINT Level, D3D9ExSurface** ppSurfaceLevel);
    virtual HRESULT LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
    virtual HRESULT UnlockRect(UINT Level);
    virtual HRESULT AddDirtyRect(const RECT* pDirtyRect);

    IDirect3DTexture9* GetD3DTexture() const
    {
        return d3dTexture;
    }
};