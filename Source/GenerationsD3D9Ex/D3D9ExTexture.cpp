#include "D3D9ExTexture.h"
#include "D3D9ExSurface.h"

D3D9ExTexture::D3D9ExTexture(D3D9ExDevice* d3dDevice, IDirect3DTexture9* d3dTexture) : D3D9ExBaseTexture(d3dDevice, d3dTexture), d3dTexture(d3dTexture)
{
    const uint32_t levelCount = d3dTexture->GetLevelCount();

    dxpSurfaces.reserve(levelCount);
    for (size_t i = 0; i < levelCount; i++)
    {
        IDirect3DSurface9* d3dSurface;
        d3dTexture->GetSurfaceLevel(i, &d3dSurface);

        dxpSurfaces.push_back(new D3D9ExSurface(d3dDevice, d3dSurface));

        d3dSurface->Release();
    }
}

D3D9ExTexture::~D3D9ExTexture()
{
    for (auto& dxpSurface : dxpSurfaces)
        dxpSurface->Release();
}

HRESULT D3D9ExTexture::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
{
    return d3dTexture->GetLevelDesc(Level, pDesc);
}

HRESULT D3D9ExTexture::GetSurfaceLevel(UINT Level, D3D9ExSurface** ppSurfaceLevel)
{
    *ppSurfaceLevel = dxpSurfaces[Level];
    (*ppSurfaceLevel)->AddRef();

    return S_OK;
}

HRESULT D3D9ExTexture::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)
{
    return d3dTexture->LockRect(Level, pLockedRect, pRect, Flags);
}

HRESULT D3D9ExTexture::UnlockRect(UINT Level)
{
    return d3dTexture->UnlockRect(Level);
}

HRESULT D3D9ExTexture::AddDirtyRect(CONST RECT* pDirtyRect)
{
    return d3dTexture->AddDirtyRect(pDirtyRect);
}