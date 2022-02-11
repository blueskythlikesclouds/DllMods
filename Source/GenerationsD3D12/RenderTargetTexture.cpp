#include "RenderTargetTexture.h"
#include "RenderTargetSurface.h"

RenderTargetTexture::RenderTargetTexture(const ComPtr<Device>& d3dDevice, const ComPtr<ID3D12Resource>& d3dResource, DXGI_FORMAT format)
    : Texture(d3dDevice, d3dResource), format(format)
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

DXGI_FORMAT RenderTargetTexture::getFormat() const
{
    return format;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetTexture::getD3DCpuDescriptorHandle() const
{
    return d3dCpuDescriptorHandle;
}

HRESULT RenderTargetTexture::GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel)
{
    if (!d3dRenderTargetSurface)
        d3dRenderTargetSurface = new RenderTargetSurface(d3dDevice, this);

    *ppSurfaceLevel = d3dRenderTargetSurface.Get();
    (*ppSurfaceLevel)->AddRef();
    return S_OK;
}
