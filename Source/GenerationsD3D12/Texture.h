#pragma once

#include "BaseTexture.h"

class Surface;

class Texture : public BaseTexture
{
    DXGI_FORMAT format{};
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

    void initialize();

public:
    explicit Texture(Device* device, ID3D12Resource* resource);
    explicit Texture(Device* device, D3D12MA::Allocation* allocation);

    DXGI_FORMAT getFormat() const;
    const D3D12_SHADER_RESOURCE_VIEW_DESC& getSrvDesc() const;

    virtual HRESULT GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc);
    virtual HRESULT GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel);
    virtual HRESULT LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
    virtual HRESULT UnlockRect(UINT Level);
    virtual HRESULT AddDirtyRect(const RECT* pDirtyRect);
};