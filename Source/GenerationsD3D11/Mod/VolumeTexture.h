#pragma once

#include "BaseTexture.h"

class VolumeTexture : public BaseTexture
{
    D3D11_TEXTURE3D_DESC desc;
    std::vector<std::vector<uint32_t>> pixels;
    std::vector<DWORD> lockFlags;

public:
    VolumeTexture(Device* device, ID3D11Texture3D* texture);

    virtual HRESULT GetLevelDesc(UINT Level, D3DVOLUME_DESC* pDesc) final;
    virtual HRESULT GetVolumeLevel(UINT Level, IDirect3DVolume9** ppVolumeLevel) final;
    virtual HRESULT LockBox(UINT Level, D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags) final;
    virtual HRESULT UnlockBox(UINT Level) final;
    virtual HRESULT AddDirtyBox(const D3DBOX* pDirtyBox) final;
};
