#pragma once

#include "Texture.h"

class DepthStencilSurface;

class DepthStencilTexture : public Texture
{
    ComPtr<ID3D11DepthStencilView> dsv;
    ComPtr<DepthStencilSurface> surface;

public:
    explicit DepthStencilTexture(Device* device, ID3D11Resource* resource, DXGI_FORMAT format);

    ID3D11DepthStencilView* getDSV() const;

    HRESULT GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel) override;
};
