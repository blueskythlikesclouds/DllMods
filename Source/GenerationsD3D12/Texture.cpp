#include "Texture.h"
#include "TypeConverter.h"

Texture::Texture(const ComPtr<Device>& device, const ComPtr<ID3D12Resource>& resource)
    : BaseTexture(device, resource)
{
    if (!resource)
        return;

    const auto resourceDesc = resource->GetDesc();
    format = resourceDesc.Format;

    D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
    desc.Format = TypeConverter::makeColor(format);
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    if (resourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
    {
        if (resourceDesc.DepthOrArraySize == 6)
        {
            desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
            desc.TextureCube.MipLevels = resourceDesc.MipLevels;
        }
        else
        {
            desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipLevels = resourceDesc.MipLevels;
        }
    }
    else if (resourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
    {
        desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
        desc.Texture3D.MipLevels = resourceDesc.MipLevels;
    }
    else
    {
        return;
    }

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.NumDescriptors = 1;

    device->getUnderlyingDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&srvDescriptorHeap));
    srvDescriptorHandle = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    device->getUnderlyingDevice()->CreateShaderResourceView(resource.Get(), &desc, srvDescriptorHandle);
}

DXGI_FORMAT Texture::getFormat() const
{
    return format;
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::getSrvDescriptorHandle() const
{
    return srvDescriptorHandle;
}

HRESULT Texture::GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc)
{
    if (!pDesc)
        return E_INVALIDARG;

    const D3D12_RESOURCE_DESC desc = resource->GetDesc();
    if (desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D)
        return E_FAIL;

    ZeroMemory(pDesc, sizeof(*pDesc));
    pDesc->Width = (UINT)desc.Width >> Level;
    pDesc->Height = (UINT)desc.Height >> Level;

    return S_OK;
}

FUNCTION_STUB(HRESULT, Texture::GetSurfaceLevel, UINT Level, Surface** ppSurfaceLevel)

FUNCTION_STUB(HRESULT, Texture::LockRect, UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)

FUNCTION_STUB(HRESULT, Texture::UnlockRect, UINT Level)

FUNCTION_STUB(HRESULT, Texture::AddDirtyRect, CONST RECT* pDirtyRect)