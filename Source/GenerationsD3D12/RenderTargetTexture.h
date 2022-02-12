#pragma once

#include "Texture.h"

class RenderTargetSurface;

class RenderTargetTexture : public Texture
{
    ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE rtvDescriptorHandle{};
    ComPtr<RenderTargetSurface> surface;

    void initialize();

public:
    explicit RenderTargetTexture(Device* device, ID3D12Resource* resource);
    explicit RenderTargetTexture(Device* device, D3D12MA::Allocation* allocation);

    D3D12_CPU_DESCRIPTOR_HANDLE getRtvDescriptorHandle() const;
        
    HRESULT GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel) override;
};