#pragma once

#include "BaseTexture.h"

class Surface;

class Texture : public BaseTexture
{
    DXGI_FORMAT format{};
    ComPtr<ID3D11ShaderResourceView> srv;

public:
    explicit Texture(Device* device, ID3D11Resource* resource, DXGI_FORMAT format);
    explicit Texture(Device* device, ID3D11Resource* resource, ID3D11ShaderResourceView* srv);

    DXGI_FORMAT getFormat() const;
    ID3D11ShaderResourceView* getSRV() const;

    virtual HRESULT GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc) final;
    virtual HRESULT GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel);
    virtual HRESULT LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) final;
    virtual HRESULT UnlockRect(UINT Level) final;
    virtual HRESULT AddDirtyRect(const RECT* pDirtyRect) final;
};