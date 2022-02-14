#pragma once

#include "Resource.h"

class Buffer : public Resource
{
    size_t length;
    D3D11_BIND_FLAG bindFlags;
    DXGI_FORMAT format;
    std::unique_ptr<uint8_t[]> uploadBuffer;

public:
    explicit Buffer(Device* device, size_t length, D3D11_BIND_FLAG bindFlags, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);
    ~Buffer() = default;

    DXGI_FORMAT getFormat() const;

    virtual HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags);
    virtual HRESULT Unlock();
    virtual HRESULT GetDesc(D3DINDEXBUFFER_DESC* pDesc);
};