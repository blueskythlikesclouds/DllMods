#include "RenderTargetTexture.h"

#include "RenderTargetSurface.h"

void RenderTargetTexture::initialize()
{
    ID3D12Resource* resource = getResource();
    if (!resource)
        return;

    // Create heap for RTV
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    device->getDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvDescriptorHeap));

    // Get CPU handle
    rtvDescriptorHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    // Create RTV without D3D12_RENDER_TARGET_VIEW_DESC
    device->getDevice()->CreateRenderTargetView(resource, nullptr, rtvDescriptorHandle);
}

RenderTargetTexture::RenderTargetTexture(Device* device, ID3D12Resource* resource)
    : Texture(device, resource)
{
    initialize();
}

RenderTargetTexture::RenderTargetTexture(Device* device, D3D12MA::Allocation* allocation)
    : Texture(device, allocation)
{
    initialize();
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetTexture::getRtvDescriptorHandle() const
{
    return rtvDescriptorHandle;
}

HRESULT RenderTargetTexture::GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel)
{
    if (!surface)
        surface = new RenderTargetSurface(device.Get(), this);

    *ppSurfaceLevel = surface.Get();
    (*ppSurfaceLevel)->AddRef();
    return S_OK;
}
