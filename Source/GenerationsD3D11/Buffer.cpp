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
    uploadBuffer = std::make_unique<uint8_t[]>(length);
    *ppbData = uploadBuffer.get();
    return S_OK;
}

HRESULT Buffer::Unlock()
{
    {
        const auto lock = device->lock();

        D3D11_BUFFER_DESC desc{};
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = length;
        desc.BindFlags = bindFlags;

        D3D11_SUBRESOURCE_DATA data{};
        data.pSysMem = uploadBuffer.get();

        device->getDevice()->CreateBuffer(&desc, &data, reinterpret_cast<ID3D11Buffer**>(resource.ReleaseAndGetAddressOf()));
    }

    uploadBuffer = nullptr;

    return S_OK;
}

FUNCTION_STUB(HRESULT, Buffer::GetDesc, D3DINDEXBUFFER_DESC *pDesc)