#include "VertexBuffer.h"
#include "Device.h"

VertexBuffer::VertexBuffer(const ComPtr<Device>& d3dDevice, size_t length)
    : Resource(d3dDevice, nullptr), buffer(nullptr), length(length)
{
}

HRESULT VertexBuffer::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
    return S_OK;
}

HRESULT VertexBuffer::Unlock()
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, VertexBuffer::GetDesc, D3DVERTEXBUFFER_DESC *pDesc)
