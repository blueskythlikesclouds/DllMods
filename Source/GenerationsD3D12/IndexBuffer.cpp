#include "IndexBuffer.h"
#include "Device.h"

IndexBuffer::IndexBuffer(const ComPtr<Device>& d3dDevice, size_t length)
    : Resource(d3dDevice, nullptr), buffer(nullptr), length(length)
{
}

HRESULT IndexBuffer::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
    return S_OK;
}

HRESULT IndexBuffer::Unlock()
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, IndexBuffer::GetDesc, D3DINDEXBUFFER_DESC *pDesc)