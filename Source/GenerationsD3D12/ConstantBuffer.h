﻿#pragma once

template<typename T>
class ConstantBuffer
{
    ComPtr<ID3D12Resource> resource;
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle{};
    T* data{};

public:
    ~ConstantBuffer()
    {
        if (resource)
            resource->Unmap(0, nullptr);
    }

    ID3D12Resource* getResource() const
    {
        return resource.Get();
    }

    ID3D12DescriptorHeap* getDescriptorHeap() const
    {
        return descriptorHeap.Get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE getCpuDescriptorHandle() const
    {
        return cpuDescriptorHandle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE getGpuDescriptorHandle() const
    {
        return gpuDescriptorHandle;
    }

    T* getData() const
    {
        return data;
    }

    void initialize(const ComPtr<ID3D12Device>& device)
    {
        // Create constant buffer.
        device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer((sizeof(T) + 255) & ~0xFF),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&resource));

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
        cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = (sizeof(T) + 255) & ~0xFF;
        device->CreateConstantBufferView(&cbvDesc, cpuDescriptorHandle);

        // Map constant buffer.
        resource->Map(0, nullptr, reinterpret_cast<void**>(&data));
    }
};