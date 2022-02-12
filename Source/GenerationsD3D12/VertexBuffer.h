#pragma once

#include "Resource.h"

class VertexBuffer : public Resource
{
    ComPtr<D3D12MA::Allocation> uploadHeap;
    size_t length;

public:
    explicit VertexBuffer(Device* device, size_t length);
    ~VertexBuffer() = default;

    D3D12_VERTEX_BUFFER_VIEW getVertexBufferView(size_t offsetInBytes, size_t stride) const;

    virtual HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags);
    virtual HRESULT Unlock();
    virtual HRESULT GetDesc(D3DVERTEXBUFFER_DESC* pDesc);
};
