#include "UploadBufferPool.h"

UploadBufferPool::~UploadBufferPool()
{
    for (auto& pair : buffers)
        pair.first->GetResource()->Unmap(0, nullptr);
}

UploadBuffer UploadBufferPool::allocate(D3D12MA::Allocator* allocator, size_t size)
{
    constexpr size_t bufferSize = 65536;
    size = (size + 255) & ~0xFF;

    const size_t currentInHeap = current % bufferSize;
    if (currentInHeap + size > bufferSize)
        current += bufferSize - currentInHeap;

    if (current + size > buffers.size() * bufferSize)
    {
        current = buffers.size() * bufferSize;

        D3D12MA::ALLOCATION_DESC desc{};
        desc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

        ComPtr<D3D12MA::Allocation> allocation;

        allocator->CreateResource(
            &desc,
            &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ | D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            nullptr,
            allocation.GetAddressOf(),
            __uuidof(ID3D12Resource),
            nullptr);

        void* data;
        allocation->GetResource()->Map(0, &CD3DX12_RANGE(0, 0), &data);

        buffers.emplace_back(std::move(allocation), data);
    }

    const auto& pair = buffers[current / bufferSize];
    
    const UploadBuffer result =
    {
        pair.first.Get(),
        (char*)pair.second + current % bufferSize,
        size,
        pair.first->GetResource()->GetGPUVirtualAddress() + current % bufferSize
    };

    current += size;

    return result;
}

void UploadBufferPool::reset()
{
    current = 0;
}
