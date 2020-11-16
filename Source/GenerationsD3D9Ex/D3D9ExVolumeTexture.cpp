#include "D3D9ExVolumeTexture.h"

D3D9ExVolumeTexture::D3D9ExVolumeTexture(D3D9ExDevice* d3dDevice, IDirect3DVolumeTexture9* d3dVolumeTexture)
    : D3D9ExBaseTexture(d3dDevice, d3dVolumeTexture), d3dVolumeTexture(d3dVolumeTexture)
{
}

D3D9ExVolumeTexture::~D3D9ExVolumeTexture() = default;

HRESULT D3D9ExVolumeTexture::GetLevelDesc(UINT Level, D3DVOLUME_DESC* pDesc)
{
    return S_OK;
}

HRESULT D3D9ExVolumeTexture::GetVolumeLevel(UINT Level, IDirect3DVolume9** ppVolumeLevel)
{
    return S_OK;
}

HRESULT D3D9ExVolumeTexture::LockBox(UINT Level, D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags)
{
    return S_OK;
}

HRESULT D3D9ExVolumeTexture::UnlockBox(UINT Level)
{
    return S_OK;
}

HRESULT D3D9ExVolumeTexture::AddDirtyBox(const D3DBOX* pDirtyBox)
{
    return S_OK;
}
