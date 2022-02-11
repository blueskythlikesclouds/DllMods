#include "DepthStencilTexture.h"
#include "DepthStencilSurface.h"

DepthStencilTexture::DepthStencilTexture(const ComPtr<Device>& d3dDevice, const ComPtr<ID3D12Resource>& d3dResource, DXGI_FORMAT format)
    : Texture(d3dDevice, d3dResource), format(format)
{
    // Create heap for DSV
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    d3dDevice->getD3DDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3dDescriptorHeap));

    // Get CPU handle
    d3dCpuDescriptorHandle = d3dDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    // Create DSV without D3D12_DEPTH_STENCIL_VIEW_DESC
    d3dDevice->getD3DDevice()->CreateDepthStencilView(d3dResource.Get(), nullptr, d3dCpuDescriptorHandle);
}

DXGI_FORMAT DepthStencilTexture::getFormat() const
{
    return format;
}

D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilTexture::getD3DCpuDescriptorHandle() const
{
    return d3dCpuDescriptorHandle;
}

HRESULT DepthStencilTexture::GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel)
{
    if (!d3dDepthStencilSurface)
        d3dDepthStencilSurface = new DepthStencilSurface(d3dDevice, this);

    *ppSurfaceLevel = d3dDepthStencilSurface.Get();
    (*ppSurfaceLevel)->AddRef();
    return S_OK;
}
