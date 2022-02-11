#include "RenderTargetTexture.h"

RenderTargetTexture::RenderTargetTexture(const ComPtr<Device>& d3dDevice, const ComPtr<ID3D12Resource>& d3dResource) : Texture(d3dDevice, d3dResource)
{
    // Create heap for RTV
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    d3dDevice->getD3DDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3dDescriptorHeap));

    // Get CPU handle
    d3dCpuDescriptorHandle = d3dDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    // Create RTV without D3D12_RENDER_TARGET_VIEW_DESC
    d3dDevice->getD3DDevice()->CreateRenderTargetView(d3dResource.Get(), nullptr, d3dCpuDescriptorHandle);
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetTexture::getD3DCpuDescriptorHandle() const
{
    return d3dCpuDescriptorHandle;
}
