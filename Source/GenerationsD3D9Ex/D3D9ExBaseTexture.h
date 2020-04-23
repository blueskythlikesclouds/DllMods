#pragma once

#include "D3D9ExResource.h"

class D3D9ExBaseTexture : public D3D9ExResource
{
    IDirect3DBaseTexture9* d3dBaseTexture;

public:
    D3D9ExBaseTexture(D3D9ExDevice* d3dDevice, IDirect3DBaseTexture9* d3dBaseTexture);

    virtual DWORD SetLOD(DWORD LODNew);
    virtual DWORD GetLOD();
    virtual DWORD GetLevelCount();
    virtual HRESULT SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType);
    virtual D3DTEXTUREFILTERTYPE GetAutoGenFilterType();
    virtual void GenerateMipSubLevels();
    virtual void FUN_48();
    virtual HRESULT FUN_4C(void* A1);

    IDirect3DBaseTexture9* GetD3DBaseTexture() const
    {
        return d3dBaseTexture;
    }
};
