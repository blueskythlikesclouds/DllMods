#pragma once

#include "BaseTexture.h"

class Surface;

class Texture : public BaseTexture
{
public:
    explicit Texture(const ComPtr<Device>& device, const ComPtr<ID3D12Resource>& resource);

    virtual HRESULT GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc);
    virtual HRESULT GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel);
    virtual HRESULT LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
    virtual HRESULT UnlockRect(UINT Level);
    virtual HRESULT AddDirtyRect(const RECT* pDirtyRect);
};