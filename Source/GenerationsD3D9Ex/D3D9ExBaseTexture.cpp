#include "D3D9ExBaseTexture.h"

D3D9ExBaseTexture::D3D9ExBaseTexture(D3D9ExDevice* d3dDevice, IDirect3DBaseTexture9* d3dBaseTexture) : D3D9ExResource(d3dDevice, d3dBaseTexture), d3dBaseTexture(d3dBaseTexture)
{
}

DWORD D3D9ExBaseTexture::SetLOD(DWORD LODNew)
{
    return d3dBaseTexture->SetLOD(LODNew);
}

DWORD D3D9ExBaseTexture::GetLOD()
{
    return d3dBaseTexture->GetLOD();
}

DWORD D3D9ExBaseTexture::GetLevelCount()
{
    return d3dBaseTexture->GetLevelCount();
}

HRESULT D3D9ExBaseTexture::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
    return d3dBaseTexture->SetAutoGenFilterType(FilterType);
}

D3DTEXTUREFILTERTYPE D3D9ExBaseTexture::GetAutoGenFilterType()
{
    return d3dBaseTexture->GetAutoGenFilterType();
}

void D3D9ExBaseTexture::GenerateMipSubLevels()
{
    return d3dBaseTexture->GenerateMipSubLevels();
}

void D3D9ExBaseTexture::FUN_48()
{
}

HRESULT D3D9ExBaseTexture::FUN_4C(void* A1)
{
    return S_OK;
}
