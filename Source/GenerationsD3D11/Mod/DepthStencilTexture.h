#pragma once

#include "Texture.h"

class DepthStencilSurface;

class DepthStencilTexture : public Texture
{
    ComPtr<DepthStencilSurface> surfaces[16]{};

public:
    explicit DepthStencilTexture(Device* device, ID3D11Resource* resource, DXGI_FORMAT format);

    HRESULT GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel) override;
};
