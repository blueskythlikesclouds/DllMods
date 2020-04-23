#include "D3D9ExCubeTexture.h"
#include "D3D9ExSurface.h"

D3D9ExCubeTexture::D3D9ExCubeTexture(D3D9ExDevice* d3dDevice, IDirect3DCubeTexture9* d3dCubeTexture) : D3D9ExBaseTexture(d3dDevice, d3dCubeTexture), d3dCubeTexture(d3dCubeTexture)
{
    const uint32_t levelCount = d3dCubeTexture->GetLevelCount();

    for (size_t i = 0; i < dxpCubeMapSurfaces.size(); i++)
    {
        dxpCubeMapSurfaces[i].reserve(levelCount);

        for (size_t j = 0; j < levelCount; j++)
        {
            IDirect3DSurface9* d3dSurface;
            d3dCubeTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)i, j, &d3dSurface);

            dxpCubeMapSurfaces[i].push_back(new D3D9ExSurface(d3dDevice, d3dSurface));

            d3dSurface->Release();
        }
    }
}

D3D9ExCubeTexture::~D3D9ExCubeTexture()
{
    for (auto& dxpSurfaces : dxpCubeMapSurfaces)
    {
        for (auto& dxpSurface : dxpSurfaces)
            dxpSurface->Release();
    }
}

HRESULT D3D9ExCubeTexture::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
{
    return d3dCubeTexture->GetLevelDesc(Level, pDesc);
}

HRESULT D3D9ExCubeTexture::GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, D3D9ExSurface** ppCubeMapSurface)
{
    *ppCubeMapSurface = dxpCubeMapSurfaces[(uint32_t)FaceType][Level];
    (*ppCubeMapSurface)->AddRef();
    return S_OK;
}

HRESULT D3D9ExCubeTexture::LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)
{
    return d3dCubeTexture->LockRect(FaceType, Level, pLockedRect, pRect, Flags);
}

HRESULT D3D9ExCubeTexture::UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level)
{
    return d3dCubeTexture->UnlockRect(FaceType, Level);
}

HRESULT D3D9ExCubeTexture::AddDirtyRect(D3DCUBEMAP_FACES FaceType, CONST RECT* pDirtyRect)
{
    return d3dCubeTexture->AddDirtyRect(FaceType, pDirtyRect);
}