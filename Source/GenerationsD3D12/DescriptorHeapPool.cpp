#include "DescriptorHeapPool.h"

DescriptorHeapPool::DescriptorHeapPool(const D3D12_DESCRIPTOR_HEAP_TYPE type) : type(type)
{
}

DescriptorHeap DescriptorHeapPool::allocate(ID3D12Device* device, size_t count)
{
    const size_t incrementSize = device->GetDescriptorHandleIncrementSize(type);
    constexpr size_t descriptorsPerHeap = 2048;

    const size_t currentInHeap = current % descriptorsPerHeap;
    if (currentInHeap + count > descriptorsPerHeap)
        current += descriptorsPerHeap - currentInHeap;
    
    if (current + count > descriptorHeaps.size() * descriptorsPerHeap)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        desc.Type = type;
        desc.NumDescriptors = descriptorsPerHeap;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        auto& descriptorHeap = descriptorHeaps.emplace_back();
        device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(descriptorHeap.GetAddressOf()));
    }

    ID3D12DescriptorHeap* descriptorHeap = descriptorHeaps[current / descriptorsPerHeap].Get();

    const DescriptorHeap result =
    {
        descriptorHeap,
        incrementSize,
        CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), current % descriptorsPerHeap, incrementSize),
        CD3DX12_GPU_DESCRIPTOR_HANDLE(descriptorHeap->GetGPUDescriptorHandleForHeapStart(), current % descriptorsPerHeap, incrementSize),
    };

    current += count;

    return result;
}

void DescriptorHeapPool::reset()
{
    current = 0;
}
