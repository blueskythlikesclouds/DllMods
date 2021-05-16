#pragma once

#include "D3D9ExBaseTexture.h"

class D3D9ExSurface;

class D3D9ExVolumeTexture : public D3D9ExBaseTexture
{
    IDirect3DVolumeTexture9* d3dVolumeTexture;

public:
    D3D9ExVolumeTexture(D3D9ExDevice* d3dDevice, IDirect3DVolumeTexture9* d3dVolumeTexture);
    ~D3D9ExVolumeTexture();

    virtual HRESULT GetLevelDesc(UINT Level, D3DVOLUME_DESC* pDesc) final;
    virtual HRESULT GetVolumeLevel(UINT Level, IDirect3DVolume9** ppVolumeLevel) final;
    virtual HRESULT LockBox(UINT Level, D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags) final;
    virtual HRESULT UnlockBox(UINT Level) final;
    virtual HRESULT AddDirtyBox(const D3DBOX* pDirtyBox) final;

    IDirect3DVolumeTexture9* GetD3DTexture() const
    {
        return d3dVolumeTexture;
    }
};