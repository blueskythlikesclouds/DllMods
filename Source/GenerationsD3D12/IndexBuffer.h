#pragma once

#include "Resource.h"

class IndexBuffer : public Resource
{
    ComPtr<ID3D12Resource> d3dUploadHeap;
    size_t length;
    D3DFORMAT format;
    ComPtr<ID3D12DescriptorHeap> d3dDescriptorHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE d3dCpuDescriptorHandle{};

public:
    explicit IndexBuffer(const ComPtr<Device>& d3dDevice, size_t length, D3DFORMAT format);
    ~IndexBuffer() = default;

    D3D12_INDEX_BUFFER_VIEW getD3DIndexBufferView() const;

    virtual HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags);
    virtual HRESULT Unlock();
    virtual HRESULT GetDesc(D3DINDEXBUFFER_DESC* pDesc);
};