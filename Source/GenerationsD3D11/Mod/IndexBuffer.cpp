#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(Device* device, size_t length, D3D11_BIND_FLAG bindFlags, DXGI_FORMAT format)
    : Buffer(device, length, bindFlags, format)
{
}

HRESULT IndexBuffer::GetDesc(D3DINDEXBUFFER_DESC* pDesc)
{
    pDesc->Format = D3DFMT_INDEX16;
    pDesc->Type = D3DRTYPE_INDEXBUFFER;
    pDesc->Usage = D3DUSAGE_WRITEONLY;
    pDesc->Pool = D3DPOOL_DEFAULT;
    pDesc->Size = length;

    return S_OK;
}
