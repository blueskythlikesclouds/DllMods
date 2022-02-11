#include "VertexBuffer.h"
#include "Device.h"

VertexBuffer::VertexBuffer(const ComPtr<Device>& d3dDevice, const size_t length)
    : Resource(d3dDevice, nullptr), length(length)
{
}

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::getD3DVertexBufferView(const size_t offsetInBytes, const size_t stride) const
{
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    vertexBufferView.BufferLocation = d3dResource->GetGPUVirtualAddress() + offsetInBytes;
    vertexBufferView.SizeInBytes = length - offsetInBytes;
    vertexBufferView.StrideInBytes = stride;
    return vertexBufferView;
}

HRESULT VertexBuffer::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
    // Create upload heap.
    d3dDevice->getD3DDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(length),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&d3dUploadHeap));

    // Lock upload heap.
    const D3D12_RANGE range = { OffsetToLock, OffsetToLock + SizeToLock };
    return d3dUploadHeap->Map(0, &range, ppbData);
}

HRESULT VertexBuffer::Unlock()
{
    // Unlock upload heap.
    d3dUploadHeap->Unmap(0, nullptr);

    // Create default heap.
    d3dDevice->getD3DDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(length),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&d3dResource));

    auto& commandQueue = d3dDevice->getCommandQueue(CommandQueueType::Load);
    const auto lock = commandQueue.lock();

    // Copy data from upload heap to default heap.
    commandQueue.getD3DCommandList()->CopyBufferRegion(
        d3dResource.Get(),
        0,
        d3dUploadHeap.Get(),
        0,
        length);

    // Transition default heap to vertex buffer state.
    commandQueue.getD3DCommandList()->ResourceBarrier(
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            d3dResource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

    // Execute command list before freeing upload heap.
    commandQueue.executeCommandList();
    commandQueue.waitForFenceEvent();
    commandQueue.resetCommandList();

    // Free upload heap.
    d3dUploadHeap = nullptr;

    return S_OK;
}

FUNCTION_STUB(HRESULT, VertexBuffer::GetDesc, D3DVERTEXBUFFER_DESC *pDesc)
