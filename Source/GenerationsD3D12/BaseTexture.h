#pragma once

#include "Resource.h"

class BaseTexture : public Resource
{
public:
    BaseTexture(const ComPtr<Device>& d3dDevice, const ComPtr<ID3D12Resource>& d3dResource);

    virtual DWORD SetLOD(DWORD LODNew);
    virtual DWORD GetLOD();
    virtual DWORD GetLevelCount();
    virtual HRESULT SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType);
    virtual D3DTEXTUREFILTERTYPE GetAutoGenFilterType();
    virtual void GenerateMipSubLevels();
    virtual void FUN_48();
    virtual HRESULT FUN_4C(void* A1);
};
