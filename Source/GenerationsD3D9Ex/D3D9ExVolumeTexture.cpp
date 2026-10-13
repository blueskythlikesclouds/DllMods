#include "D3D9ExVolumeTexture.h"

D3D9ExVolumeTexture::D3D9ExVolumeTexture(D3D9ExDevice* d3dDevice, IDirect3DVolumeTexture9* d3dVolumeTexture)
    : D3D9ExBaseTexture(d3dDevice, d3dVolumeTexture), d3dVolumeTexture(d3dVolumeTexture)
{
}

D3D9ExVolumeTexture::~D3D9ExVolumeTexture() = default;

HRESULT D3D9ExVolumeTexture::GetLevelDesc(UINT Level, D3DVOLUME_DESC* pDesc)
{
    return d3dVolumeTexture->GetLevelDesc(Level, pDesc);
}

HRESULT D3D9ExVolumeTexture::GetVolumeLevel(UINT Level, IDirect3DVolume9** ppVolumeLevel)
{
    return d3dVolumeTexture->GetVolumeLevel(Level, ppVolumeLevel);
}

HRESULT D3D9ExVolumeTexture::LockBox(UINT Level, D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags)
{
    return d3dVolumeTexture->LockBox(Level, pLockedVolume, pBox, Flags);
}

HRESULT D3D9ExVolumeTexture::UnlockBox(UINT Level)
{
    return d3dVolumeTexture->UnlockBox(Level);
}

HRESULT D3D9ExVolumeTexture::AddDirtyBox(const D3DBOX* pDirtyBox)
{
    return d3dVolumeTexture->AddDirtyBox(pDirtyBox);
}
