#include "VertexBuffer.h"

#include "Device.h"

VertexBuffer::VertexBuffer(Device* device, const size_t length)
    : Resource(device, (ID3D12Resource*)nullptr), length(length)
{
}

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::getVertexBufferView(const size_t offsetInBytes, const size_t stride) const
{
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    vertexBufferView.BufferLocation = allocation->GetResource()->GetGPUVirtualAddress() + offsetInBytes;
    vertexBufferView.SizeInBytes = length - offsetInBytes;
    vertexBufferView.StrideInBytes = stride;
    return vertexBufferView;
}

HRESULT VertexBuffer::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
    // Create upload heap.
    device->createResource(
        D3D12_HEAP_TYPE_UPLOAD,
        &CD3DX12_RESOURCE_DESC::Buffer(length),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        &uploadHeap);

    // Lock upload heap.
    const D3D12_RANGE range = { OffsetToLock, OffsetToLock + SizeToLock };
    return uploadHeap->GetResource()->Map(0, &range, ppbData);
}

HRESULT VertexBuffer::Unlock()
{
    // Unlock upload heap.
    uploadHeap->GetResource()->Unmap(0, nullptr);

    // Create default heap.
    device->createResource(
        D3D12_HEAP_TYPE_DEFAULT,
        &CD3DX12_RESOURCE_DESC::Buffer(length),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        &allocation);

    auto& queue = device->getLoadQueue();
    const auto lock = queue.lock();

    // Copy data from upload heap to default heap.
    queue.getCommandList()->CopyBufferRegion(
        allocation->GetResource(),
        0,
        uploadHeap->GetResource(),
        0,
        length);

    // Transition default heap to index buffer state.
    queue.getCommandList()->ResourceBarrier(
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            allocation->GetResource(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

    // Execute command list before freeing upload heap.
    queue.submitAll();

    // Free upload heap.
    uploadHeap = nullptr;

    return S_OK;
}

FUNCTION_STUB(HRESULT, VertexBuffer::GetDesc, D3DVERTEXBUFFER_DESC *pDesc)
