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

    virtual HRESULT GetDevice(D3D9ExDevice** ppDevice);
    virtual HRESULT SetPrivateData(const GUID& refguid, const void* pData, DWORD SizeOfData, DWORD Flags);
    virtual HRESULT GetPrivateData(const GUID& refguid, void* pData, DWORD* pSizeOfData);
    virtual HRESULT FreePrivateData(const GUID& refguid);
    virtual DWORD SetPriority(DWORD PriorityNew);
    virtual DWORD GetPriority();
    virtual void PreLoad();
    virtual D3DRESOURCETYPE GetType();

    IDirect3DResource9* GetD3DResource() const
    {
        return d3dResource;
    }
};