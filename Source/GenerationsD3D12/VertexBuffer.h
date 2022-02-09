#pragma once

#include "Resource.h"

class VertexBuffer : public Resource
{
    std::unique_ptr<uint8_t[]> buffer;
    size_t length;

public:
    explicit VertexBuffer(const ComPtr<Device>& d3dDevice, size_t length);
    ~VertexBuffer() = default;

    virtual HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags);
    virtual HRESULT Unlock();
    virtual HRESULT GetDesc(D3DVERTEXBUFFER_DESC* pDesc);
};
