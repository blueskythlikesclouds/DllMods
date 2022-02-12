#pragma once

#include "Resource.h"

class BaseTexture : public Resource
{
public:
    BaseTexture(Device* device, ID3D12Resource* resource);
    BaseTexture(Device* device, D3D12MA::Allocation* allocation);

    virtual DWORD SetLOD(DWORD LODNew);
    virtual DWORD GetLOD();
    virtual DWORD GetLevelCount();
    virtual HRESULT SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType);
    virtual D3DTEXTUREFILTERTYPE GetAutoGenFilterType();
    virtual void GenerateMipSubLevels();
    virtual void FUN_48();
    virtual HRESULT FUN_4C(void* A1);
};
