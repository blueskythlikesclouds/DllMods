#pragma once

#include "D3D9ExUnknown.h"

class D3D9ExDevice;

class D3D9ExResource : public D3D9ExUnknown
{
    uint8_t gap[8];

protected:
    D3D9ExDevice* dxpDevice;
    IDirect3DResource9* d3dResource;

public:
    D3D9ExResource(D3D9ExDevice* d3dDevice, IDirect3DResource9* d3dResource);
    ~D3D9ExResource();

    virtual HRESULT GetDevice(D3D9ExDevice** ppDevice) final;
    virtual HRESULT SetPrivateData(const GUID& refguid, const void* pData, DWORD SizeOfData, DWORD Flags) final;
    virtual HRESULT GetPrivateData(const GUID& refguid, void* pData, DWORD* pSizeOfData) final;
    virtual HRESULT FreePrivateData(const GUID& refguid) final;
    virtual DWORD SetPriority(DWORD PriorityNew) final;
    virtual DWORD GetPriority() final;
    virtual void PreLoad() final;
    virtual D3DRESOURCETYPE GetType() final;

    IDirect3DResource9* GetD3DResource() const
    {
        return d3dResource;
    }
};