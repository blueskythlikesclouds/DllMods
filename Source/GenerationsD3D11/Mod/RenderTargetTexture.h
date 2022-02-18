#pragma once

#include "Texture.h"

class RenderTargetSurface;

class RenderTargetTexture : public Texture
{
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr<RenderTargetSurface> surface;

public:
    explicit RenderTargetTexture(Device* device, ID3D11Resource* resource, DXGI_FORMAT format);

    ID3D11RenderTargetView* getRTV() const;

    HRESULT GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel) override;
};