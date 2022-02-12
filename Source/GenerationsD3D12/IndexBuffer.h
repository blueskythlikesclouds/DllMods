#pragma once

#include "Resource.h"

class IndexBuffer : public Resource
{
    ComPtr<D3D12MA::Allocation> uploadHeap;
    size_t length;
    DXGI_FORMAT format;
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle{};

public:
    explicit IndexBuffer(Device* device, size_t length, DXGI_FORMAT format);
    ~IndexBuffer() = default;

    D3D12_INDEX_BUFFER_VIEW getIndexBufferView() const;

    virtual HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags);
    virtual HRESULT Unlock();
    virtual HRESULT GetDesc(D3DINDEXBUFFER_DESC* pDesc);
};