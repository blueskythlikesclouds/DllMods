#include "VertexBuffer.h"
#include "Device.h"

VertexBuffer::VertexBuffer(const ComPtr<Device>& device, const size_t length)
    : Resource(device, nullptr), length(length)
{
}

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::getVertexBufferView(const size_t offsetInBytes, const size_t stride) const
{
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    vertexBufferView.BufferLocation = resource->GetGPUVirtualAddress() + offsetInBytes;
    vertexBufferView.SizeInBytes = length - offsetInBytes;
    vertexBufferView.StrideInBytes = stride;
    return vertexBufferView;
}

HRESULT VertexBuffer::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
    // Create upload heap.
    device->getUnderlyingDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(length),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadHeap));

    // Lock upload heap.
    const D3D12_RANGE range = { OffsetToLock, OffsetToLock + SizeToLock };
    return uploadHeap->Map(0, &range, ppbData);
}

HRESULT VertexBuffer::Unlock()
{
    // Unlock upload heap.
    uploadHeap->Unmap(0, nullptr);

    // Create default heap.
    device->getUnderlyingDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(length),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&resource));

    auto& queue = device->getLoadQueue();
    const auto lock = queue.lock();

    // Copy data from upload heap to default heap.
    queue.getCommandList()->CopyBufferRegion(
        resource.Get(),
        0,
        uploadHeap.Get(),
        0,
        length);

    // Transition default heap to vertex buffer state.
    queue.getCommandList()->ResourceBarrier(
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            resource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

    // Execute command list before freeing upload heap.
    queue.executeCommandList();
    queue.waitForFenceEvent();
    queue.resetCommandList();

    // Free upload heap.
    uploadHeap = nullptr;

    return S_OK;
}

FUNCTION_STUB(HRESULT, VertexBuffer::GetDesc, D3DVERTEXBUFFER_DESC *pDesc)
