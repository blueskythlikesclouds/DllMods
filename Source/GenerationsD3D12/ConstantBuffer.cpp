#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer(ID3D12Device* device, D3D12MA::Allocator* allocator, const size_t constantBufferSize)
{
    // Create constant buffer.
    D3D12MA::ALLOCATION_DESC allocDesc{};
    allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

    allocator->CreateResource(
        &allocDesc,
        &CD3DX12_RESOURCE_DESC::Buffer((constantBufferSize + 255) & ~0xFF),
        D3D12_RESOURCE_STATE_GENERIC_READ | D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
        nullptr,
        &buffer,
        __uuidof(ID3D12Resource),
        nullptr);

    // Create descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));

    cpuDescriptorHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    gpuDescriptorHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();

    // Create constant buffer view.
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = buffer->GetResource()->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = (constantBufferSize + 255) & ~0xFF;
    device->CreateConstantBufferView(&cbvDesc, cpuDescriptorHandle);

    // Map constant buffer.
    buffer->GetResource()->Map(0, &CD3DX12_RANGE(0, 0), &data);
}

ConstantBuffer::~ConstantBuffer()
{
    if (buffer)
        buffer->GetResource()->Unmap(0, nullptr);
}

ID3D12Resource* ConstantBuffer::getResource() const
{
    return buffer->GetResource();
}

ID3D12DescriptorHeap* ConstantBuffer::getDescriptorHeap() const
{
    return descriptorHeap.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::getCpuDescriptorHandle() const
{
    return cpuDescriptorHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE ConstantBuffer::getGpuDescriptorHandle() const
{
    return gpuDescriptorHandle;
}

void* ConstantBuffer::getData() const
{
    return data;
}