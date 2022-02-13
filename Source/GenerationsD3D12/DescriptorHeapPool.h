#pragma once

struct DescriptorHeap
{
    ID3D12DescriptorHeap* descriptorHeap;
    size_t incrementSize;
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;
};

class DescriptorHeapPool
{
    D3D12_DESCRIPTOR_HEAP_TYPE type;
    std::vector<ComPtr<ID3D12DescriptorHeap>> descriptorHeaps;
    size_t current{};

public:
    DescriptorHeapPool(D3D12_DESCRIPTOR_HEAP_TYPE type);

    DescriptorHeap allocate(ID3D12Device* device, size_t count);
    void reset();
};
