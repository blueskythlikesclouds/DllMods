#pragma once

#include "BaseTexture.h"

class Surface;

class Texture : public BaseTexture
{
    DXGI_FORMAT format{};
    ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE srvDescriptorHandle{};

public:
    explicit Texture(const ComPtr<Device>& device, const ComPtr<ID3D12Resource>& resource);

    DXGI_FORMAT getFormat() const;
    D3D12_CPU_DESCRIPTOR_HANDLE getSrvDescriptorHandle() const;

    virtual HRESULT GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc);
    virtual HRESULT GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel);
    virtual HRESULT LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
    virtual HRESULT UnlockRect(UINT Level);
    virtual HRESULT AddDirtyRect(const RECT* pDirtyRect);
};