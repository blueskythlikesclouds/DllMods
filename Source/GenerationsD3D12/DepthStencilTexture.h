#pragma once

#include "Texture.h"

class DepthStencilTexture : public Texture
{
    ComPtr<ID3D12DescriptorHeap> d3dDescriptorHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE d3dCpuDescriptorHandle{};

public:
    explicit DepthStencilTexture(const ComPtr<Device>& d3dDevice, const ComPtr<ID3D12Resource>& d3dResource);

    D3D12_CPU_DESCRIPTOR_HANDLE getD3DCpuDescriptorHandle() const;
};
