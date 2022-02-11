#pragma once

#include "Resource.h"

class VertexBuffer : public Resource
{
    ComPtr<ID3D12Resource> d3dUploadHeap;
    size_t length;

public:
    explicit VertexBuffer(const ComPtr<Device>& d3dDevice, size_t length);
    ~VertexBuffer() = default;

    D3D12_VERTEX_BUFFER_VIEW getD3DVertexBufferView(size_t offsetInBytes, size_t stride) const;

    virtual HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags);
    virtual HRESULT Unlock();
    virtual HRESULT GetDesc(D3DVERTEXBUFFER_DESC* pDesc);
};
