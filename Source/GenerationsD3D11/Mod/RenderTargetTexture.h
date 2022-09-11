#pragma once

#include "Texture.h"

class RenderTargetSurface;

class RenderTargetTexture : public Texture
{
    ComPtr<RenderTargetSurface> surfaces[16]{};

public:
    explicit RenderTargetTexture(Device* device, ID3D11Resource* resource, DXGI_FORMAT format);

    HRESULT GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel) override;
};