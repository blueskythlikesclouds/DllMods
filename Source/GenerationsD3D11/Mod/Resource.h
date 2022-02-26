#pragma once

#include "Device.h"
#include "Unknown.h"

class Resource : public Unknown
{
protected:
    ComPtr<Device> device;
    ComPtr<ID3D11Resource> resource;

public:
    explicit Resource(Device* device, ID3D11Resource* resource);

    ID3D11Resource* getResource() const;

    virtual HRESULT GetDevice(Device** ppDevice) final;
    virtual HRESULT SetPrivateData(const GUID& refguid, const void* pData, DWORD SizeOfData, DWORD Flags) final;
    virtual HRESULT GetPrivateData(const GUID& refguid, void* pData, DWORD* pSizeOfData) final;
    virtual HRESULT FreePrivateData(const GUID& refguid) final;
    virtual DWORD SetPriority(DWORD PriorityNew) final;
    virtual DWORD GetPriority() final;
    virtual void PreLoad() final;
    virtual D3DRESOURCETYPE GetType() final;
};