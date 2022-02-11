#pragma once

#include "Texture.h"

class RenderTargetSurface;

class RenderTargetTexture : public Texture
{
    DXGI_FORMAT format;
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle{};
    ComPtr<RenderTargetSurface> surface;

public:
    explicit RenderTargetTexture(const ComPtr<Device>& device, const ComPtr<ID3D12Resource>& resource);

    DXGI_FORMAT getFormat() const;
    D3D12_CPU_DESCRIPTOR_HANDLE getDescriptorHandle() const;
        
    HRESULT GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel) override;
};