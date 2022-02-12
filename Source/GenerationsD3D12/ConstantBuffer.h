#pragma once

class ConstantBuffer
{
    ComPtr<D3D12MA::Allocation> buffer;
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle{};
    void* data{};

public:
    ConstantBuffer(ID3D12Device* device, D3D12MA::Allocator* allocator, size_t constantBufferSize);
    ~ConstantBuffer();

    ID3D12Resource* getResource() const;
    ID3D12DescriptorHeap* getDescriptorHeap() const;
    D3D12_CPU_DESCRIPTOR_HANDLE getCpuDescriptorHandle() const;
    D3D12_GPU_DESCRIPTOR_HANDLE getGpuDescriptorHandle() const;

    void* getData() const;

    template<typename T>
    T* getData() const
    {
        return reinterpret_cast<T*>(data);
    }
};