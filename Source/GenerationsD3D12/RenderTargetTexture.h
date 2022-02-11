#pragma once

#include "Texture.h"

class RenderTargetSurface;

class RenderTargetTexture : public Texture
{
    ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE rtvDescriptorHandle{};
    ComPtr<RenderTargetSurface> surface;

public:
    explicit RenderTargetTexture(const ComPtr<Device>& device, const ComPtr<ID3D12Resource>& resource);

    D3D12_CPU_DESCRIPTOR_HANDLE getRtvDescriptorHandle() const;
        
    HRESULT GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel) override;
};