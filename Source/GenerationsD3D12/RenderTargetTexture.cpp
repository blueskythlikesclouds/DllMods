#include "RenderTargetTexture.h"
#include "RenderTargetSurface.h"

RenderTargetTexture::RenderTargetTexture(const ComPtr<Device>& device, const ComPtr<ID3D12Resource>& resource)
    : Texture(device, resource), format(resource->GetDesc().Format)
{
    // Create heap for RTV
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    device->getUnderlyingDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));

    // Get CPU handle
    descriptorHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

    // Create RTV without D3D12_RENDER_TARGET_VIEW_DESC
    device->getUnderlyingDevice()->CreateRenderTargetView(resource.Get(), nullptr, descriptorHandle);
}

DXGI_FORMAT RenderTargetTexture::getFormat() const
{
    return format;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetTexture::getDescriptorHandle() const
{
    return descriptorHandle;
}

HRESULT RenderTargetTexture::GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel)
{
    if (!surface)
        surface = new RenderTargetSurface(device, this);

    *ppSurfaceLevel = surface.Get();
    (*ppSurfaceLevel)->AddRef();
    return S_OK;
}
