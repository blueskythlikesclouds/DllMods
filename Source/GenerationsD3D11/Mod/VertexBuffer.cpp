#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(Device* device, size_t length, D3D11_BIND_FLAG bindFlags)
    : Buffer(device, length, bindFlags)
{
}

HRESULT VertexBuffer::GetDesc(D3DVERTEXBUFFER_DESC* pDesc)
{
    pDesc->Format = D3DFMT_UNKNOWN;
    pDesc->Type = D3DRTYPE_VERTEXBUFFER;
    pDesc->Usage = D3DUSAGE_WRITEONLY;
    pDesc->Pool = D3DPOOL_DEFAULT;
    pDesc->Size = length;
    pDesc->FVF = NULL;

    return S_OK;
}
