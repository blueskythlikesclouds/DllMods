#pragma once

#include "Device.h"
#include "Unknown.h"

class Resource : public Unknown
{
    INSERT_PADDING(8);

protected:
    ComPtr<Device> device;
    ComPtr<D3D12MA::Allocation> allocation;
    ComPtr<ID3D12Resource> resource;

public:
    explicit Resource(Device* device, ID3D12Resource* resource);
    explicit Resource(Device* device, D3D12MA::Allocation* allocation);

    ~Resource();

    D3D12MA::Allocation* getAllocation() const;
    ID3D12Resource* getResource() const;

    virtual HRESULT GetDevice(Device** ppDevice);
    virtual HRESULT SetPrivateData(const GUID& refguid, const void* pData, DWORD SizeOfData, DWORD Flags);
    virtual HRESULT GetPrivateData(const GUID& refguid, void* pData, DWORD* pSizeOfData);
    virtual HRESULT FreePrivateData(const GUID& refguid);
    virtual DWORD SetPriority(DWORD PriorityNew);
    virtual DWORD GetPriority();
    virtual void PreLoad();
    virtual D3DRESOURCETYPE GetType();
};