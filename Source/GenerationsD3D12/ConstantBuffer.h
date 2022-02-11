#pragma once

template<typename T>
class ConstantBuffer
{
    ComPtr<ID3D12Resource> d3dResource;
    ComPtr<ID3D12DescriptorHeap> d3dDescriptorHeap;
    T* data{};

public:
    ~ConstantBuffer()
    {
        if (d3dResource)
            d3dResource->Unmap(0, nullptr);
    }

    ID3D12Resource* getD3DResource() const
    {
        return d3dResource.Get();
    }

    T* getData() const
    {
        return data;
    }

    void initialize(const ComPtr<ID3D12Device>& d3dDevice)
    {
        // Create constant buffer.
        d3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer((sizeof(T) + 255) & ~0xFF),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&d3dResource));

        // Create descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = 1;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3dDescriptorHeap));

        // Create constant buffer view.
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = d3dResource->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = (sizeof(T) + 255) & ~0xFF;
        d3dDevice->CreateConstantBufferView(&cbvDesc, d3dDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

        // Map constant buffer.
        d3dResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
    }
};
