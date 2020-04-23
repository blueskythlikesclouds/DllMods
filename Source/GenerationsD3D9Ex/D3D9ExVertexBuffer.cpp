#include "D3D9ExVertexBuffer.h"

D3D9ExVertexBuffer::D3D9ExVertexBuffer(D3D9ExDevice* d3dDevice, IDirect3DVertexBuffer9* d3dVertexBuffer) : D3D9ExResource(d3dDevice, d3dVertexBuffer), d3dVertexBuffer(d3dVertexBuffer)
{
}

HRESULT D3D9ExVertexBuffer::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
    return d3dVertexBuffer->Lock(OffsetToLock, SizeToLock, ppbData, Flags);
}

HRESULT D3D9ExVertexBuffer::Unlock()
{
    return d3dVertexBuffer->Unlock();
}

HRESULT D3D9ExVertexBuffer::GetDesc(D3DVERTEXBUFFER_DESC* pDesc)
{
    return d3dVertexBuffer->GetDesc(pDesc);
}