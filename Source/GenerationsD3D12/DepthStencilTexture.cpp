#include "DepthStencilTexture.h"
#include "DepthStencilSurface.h"

DepthStencilTexture::DepthStencilTexture(const ComPtr<Device>& device, const ComPtr<ID3D12Resource>& resource)
    : Texture(device, resource), format(resource->GetDesc().Format)
{
    // Create heap for DSV
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    device->getUnderlyingDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));

    // Get CPU handle
    descriptorHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

    // Create DSV without D3D12_DEPTH_STENCIL_VIEW_DESC
    device->getUnderlyingDevice()->CreateDepthStencilView(resource.Get(), nullptr, descriptorHandle);
}

DXGI_FORMAT DepthStencilTexture::getFormat() const
{
    return format;
}

D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilTexture::getDescriptorHandle() const
{
    return descriptorHandle;
}

HRESULT DepthStencilTexture::GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel)
{
    if (!surface)
        surface = new DepthStencilSurface(device, this);

    *ppSurfaceLevel = surface.Get();
    (*ppSurfaceLevel)->AddRef();
    return S_OK;
}
