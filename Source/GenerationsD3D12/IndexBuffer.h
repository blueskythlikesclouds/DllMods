#pragma once

#include "Resource.h"

class IndexBuffer : public Resource
{
    std::unique_ptr<uint8_t[]> buffer;
    size_t length;

public:
    explicit IndexBuffer(const ComPtr<Device>& d3dDevice, size_t length);
    ~IndexBuffer() = default;

    virtual HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags);
    virtual HRESULT Unlock();
    virtual HRESULT GetDesc(D3DINDEXBUFFER_DESC* pDesc);
};