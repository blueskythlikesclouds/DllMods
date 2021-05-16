#pragma once

#include "D3D9ExResource.h"

class D3D9ExBaseTexture : public D3D9ExResource
{
    IDirect3DBaseTexture9* d3dBaseTexture;

public:
    D3D9ExBaseTexture(D3D9ExDevice* d3dDevice, IDirect3DBaseTexture9* d3dBaseTexture);

    virtual DWORD SetLOD(DWORD LODNew) final;
    virtual DWORD GetLOD() final;
    virtual DWORD GetLevelCount() final;
    virtual HRESULT SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType) final;
    virtual D3DTEXTUREFILTERTYPE GetAutoGenFilterType() final;
    virtual void GenerateMipSubLevels() final;
    virtual void FUN_48() final;
    virtual HRESULT FUN_4C(void* A1) final;

    IDirect3DBaseTexture9* GetD3DBaseTexture() const
    {
        return d3dBaseTexture;
    }
};
