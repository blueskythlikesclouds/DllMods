#include "Buffer.h"
#include "Device.h"

Buffer::Buffer(Device* device, const size_t length, const D3D11_BIND_FLAG bindFlags, const DXGI_FORMAT format)
    : Resource(device, nullptr), length(length), bindFlags(bindFlags), format(format)
{
}

DXGI_FORMAT Buffer::getFormat() const
{
    return format;
}

HRESULT Buffer::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
    // If a buffer was already created before and the game requests to lock it again,
    // recreate it as a dynamic buffer, and use Map/Unmap to update it.
    if (resource && !map)
    {
        D3D11_BUFFER_DESC desc{};
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = length;
        desc.BindFlags = bindFlags;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        device->get()->CreateBuffer(&desc, nullptr, reinterpret_cast<ID3D11Buffer**>(resource.ReleaseAndGetAddressOf()));

        map = true;
    }

    if (map)
    {
        const auto lock = device->lock();

        D3D11_MAPPED_SUBRESOURCE mappedSubResource;
        device->getContext()->Map(resource.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);

        *ppbData = mappedSubResource.pData;
    }

    else 
    {
        uploadBuffer = std::make_unique<uint8_t[]>(length);
        *ppbData = uploadBuffer.get();
    }

    return S_OK;
}

HRESULT Buffer::Unlock()
{
    if (map)
    {
        const auto lock = device->lock();
        device->getContext()->Unmap(resource.Get(), 0);
    }

    else 
    {
        D3D11_BUFFER_DESC desc{};
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.ByteWidth = length;
        desc.BindFlags = bindFlags;

        D3D11_SUBRESOURCE_DATA data{};
        data.pSysMem = uploadBuffer.get();

        device->get()->CreateBuffer(&desc, &data, reinterpret_cast<ID3D11Buffer**>(resource.ReleaseAndGetAddressOf()));

        uploadBuffer = nullptr;
    }

    return S_OK;
}

FUNCTION_STUB(HRESULT, Buffer::GetDesc, D3DINDEXBUFFER_DESC *pDesc)