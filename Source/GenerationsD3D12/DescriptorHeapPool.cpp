#include "DescriptorHeapPool.h"

DescriptorHeapPool::DescriptorHeapPool(D3D12_DESCRIPTOR_HEAP_TYPE type, size_t numDescriptors)
    : type(type), numDescriptors(numDescriptors)
{
}

ID3D12DescriptorHeap* DescriptorHeapPool::allocate(ID3D12Device* device)
{
    if (!descriptorHeaps.empty())
        ++current;

    if (current < descriptorHeaps.size())
        return descriptorHeaps[current].Get();

    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.Type = type;
    desc.NumDescriptors = numDescriptors;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    auto& descriptorHeap = descriptorHeaps.emplace_back();
    device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(descriptorHeap.GetAddressOf()));

    return descriptorHeap.Get();
}

void DescriptorHeapPool::reset()
{
    current = 0;
}
