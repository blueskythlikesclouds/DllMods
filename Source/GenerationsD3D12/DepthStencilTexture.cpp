#include "DepthStencilTexture.h"
#include "DepthStencilSurface.h"

DepthStencilTexture::DepthStencilTexture(Device* device, D3D12MA::Allocation* allocation)
    : Texture(device, allocation)
{
    // Create heap for DSV
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    device->getDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&dsvDescriptorHeap));

    // Get CPU handle
    dsvDescriptorHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    // Create DSV without D3D12_DEPTH_STENCIL_VIEW_DESC
    device->getDevice()->CreateDepthStencilView(allocation->GetResource(), nullptr, dsvDescriptorHandle);
}

D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilTexture::getDsvDescriptorHandle() const
{
    return dsvDescriptorHandle;
}

HRESULT DepthStencilTexture::GetSurfaceLevel(UINT Level, Surface** ppSurfaceLevel)
{
    if (!surface)
        surface = new DepthStencilSurface(device.Get(), this);

    *ppSurfaceLevel = surface.Get();
    (*ppSurfaceLevel)->AddRef();
    return S_OK;
}
