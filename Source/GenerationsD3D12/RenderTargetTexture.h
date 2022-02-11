#pragma once

#include "Texture.h"

class RenderTargetSurface;

class RenderTargetTexture : public Texture
{
    DXGI_FORMAT format;
    ComPtr<ID3D12DescriptorHeap> d3dDescriptorHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE d3dCpuDescriptorHandle{};
    ComPtr<RenderTargetSurface> d3dRenderTargetSurface;

public:
    explicit RenderTargetTexture(const ComPtr<Device>& d3dDevice, const ComPtr<ID3D12Resource>& d3dResource, DXGI_FORMAT format);

    DXGI_FORMAT getFormat() const;
    D3D12_CPU_DESCRIPTOR_HANDLE getD3DCpuDescriptorHandle() const;
        
    HRESULT GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel) override;
};