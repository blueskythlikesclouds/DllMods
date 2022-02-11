#pragma once

#include "Device.h"
#include "Unknown.h"

class Resource : public Unknown
{
    INSERT_PADDING(8);

protected:
    ComPtr<Device> d3dDevice;
    ComPtr<ID3D12Resource> d3dResource;

public:
    explicit Resource(const ComPtr<Device>& d3dDevice, const ComPtr<ID3D12Resource>& d3dResource);
    ~Resource();

    ID3D12Resource* getD3DResource() const;

    virtual HRESULT GetDevice(Device** ppDevice);
    virtual HRESULT SetPrivateData(const GUID& refguid, const void* pData, DWORD SizeOfData, DWORD Flags);
    virtual HRESULT GetPrivateData(const GUID& refguid, void* pData, DWORD* pSizeOfData);
    virtual HRESULT FreePrivateData(const GUID& refguid);
    virtual DWORD SetPriority(DWORD PriorityNew);
    virtual DWORD GetPriority();
    virtual void PreLoad();
    virtual D3DRESOURCETYPE GetType();
};