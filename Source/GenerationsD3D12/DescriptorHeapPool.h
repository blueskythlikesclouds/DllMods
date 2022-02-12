#pragma once

class DescriptorHeapPool
{
    D3D12_DESCRIPTOR_HEAP_TYPE type;
    size_t numDescriptors;

    std::vector<ComPtr<ID3D12DescriptorHeap>> descriptorHeaps;
    size_t current{};
public:
    DescriptorHeapPool(D3D12_DESCRIPTOR_HEAP_TYPE type, size_t numDescriptors);

    ID3D12DescriptorHeap* allocate(ID3D12Device* device);
    void reset();
};
