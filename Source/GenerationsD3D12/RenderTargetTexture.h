#pragma once

#include "Texture.h"

class RenderTargetTexture : public Texture
{
    D3D12_CPU_DESCRIPTOR_HANDLE d3d12CpuDescriptorHandle;
public:
    explicit RenderTargetTexture(const ComPtr<Device>& d3dDevice, const ComPtr<ID3D12Resource>& d3d12Resource,
                                 const D3D12_CPU_DESCRIPTOR_HANDLE& d3d12CpuDescriptorHandle);
};
