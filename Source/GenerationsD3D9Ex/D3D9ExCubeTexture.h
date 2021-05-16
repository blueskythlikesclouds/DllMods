#pragma once

#include "D3D9ExBaseTexture.h"

class D3D9ExSurface;

class D3D9ExCubeTexture : public D3D9ExBaseTexture
{
    IDirect3DCubeTexture9* d3dCubeTexture;
    std::array<std::vector<D3D9ExSurface*>, 6> dxpCubeMapSurfaces;

public:
    D3D9ExCubeTexture(D3D9ExDevice* d3dDevice, IDirect3DCubeTexture9* d3dCubeTexture);
    ~D3D9ExCubeTexture();

    virtual HRESULT GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc) final;
    virtual HRESULT GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, D3D9ExSurface** ppCubeMapSurface) final;
    virtual HRESULT LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) final;
    virtual HRESULT UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level) final;
    virtual HRESULT AddDirtyRect(D3DCUBEMAP_FACES FaceType, const RECT* pDirtyRect) final;

    IDirect3DCubeTexture9* GetD3DCubeTexture() const
    {
        return d3dCubeTexture;
    }
};