#include "D3D9ExIndexBuffer.h"

D3D9ExIndexBuffer::D3D9ExIndexBuffer(D3D9ExDevice* d3dDevice, IDirect3DIndexBuffer9* d3dIndexBuffer) : D3D9ExResource(d3dDevice, d3dIndexBuffer), d3dIndexBuffer(d3dIndexBuffer)
{
}

HRESULT D3D9ExIndexBuffer::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
    return d3dIndexBuffer->Lock(OffsetToLock, SizeToLock, ppbData, Flags);
}

HRESULT D3D9ExIndexBuffer::Unlock()
{
    return d3dIndexBuffer->Unlock();
}

HRESULT D3D9ExIndexBuffer::GetDesc(D3DINDEXBUFFER_DESC* pDesc)
{
    return d3dIndexBuffer->GetDesc(pDesc);
}