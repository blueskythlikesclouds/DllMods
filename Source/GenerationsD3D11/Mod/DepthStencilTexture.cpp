#include "DepthStencilTexture.h"

#include "DepthStencilSurface.h"

DepthStencilTexture::DepthStencilTexture(Device* device, ID3D11Resource* resource, DXGI_FORMAT format)
    : Texture(device, resource, format)
{
}

HRESULT DepthStencilTexture::GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel)
{
    if (!surfaces[Level])
        surfaces[Level].Attach(new DepthStencilSurface(device.Get(), this, Level));

    surfaces[Level]->AddRef();
    *ppSurfaceLevel = surfaces[Level].Get();

    return S_OK;
}
