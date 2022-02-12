#pragma once

#include "Texture.h"

class DepthStencilSurface;

class DepthStencilTexture : public Texture
{
    ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE dsvDescriptorHandle{};
    ComPtr<DepthStencilSurface> surface;

public:
    explicit DepthStencilTexture(Device* device, D3D12MA::Allocation* allocation);

    D3D12_CPU_DESCRIPTOR_HANDLE getDsvDescriptorHandle() const;

    HRESULT GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel) override;
};
