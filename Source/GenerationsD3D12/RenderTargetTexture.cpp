#include "RenderTargetTexture.h"

RenderTargetTexture::RenderTargetTexture(const ComPtr<Device>& d3dDevice, const ComPtr<ID3D12Resource>& d3d12Resource,
    const D3D12_CPU_DESCRIPTOR_HANDLE& d3d12CpuDescriptorHandle) : Texture(d3dDevice, d3d12Resource), d3d12CpuDescriptorHandle(d3d12CpuDescriptorHandle)
{
}
