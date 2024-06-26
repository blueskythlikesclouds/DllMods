﻿#include "RenderTargetTexture.h"

#include "RenderTargetSurface.h"

RenderTargetTexture::RenderTargetTexture(Device* device, ID3D11Resource* resource, DXGI_FORMAT format)
    : Texture(device, resource, format)
{
}

RenderTargetTexture::RenderTargetTexture(Device* device, ID3D11Resource* resource, ID3D11ShaderResourceView* srv)
    : Texture(device, resource, srv)
{
}

HRESULT RenderTargetTexture::GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel)
{
    if (!surfaces[Level])
        surfaces[Level].Attach(new RenderTargetSurface(device.Get(), this, Level));

    surfaces[Level]->AddRef();
    *ppSurfaceLevel = surfaces[Level].Get();

    return S_OK;
}
