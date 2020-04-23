#pragma once

#include "D3D9ExResource.h"

class D3D9ExVertexBuffer : public D3D9ExResource
{
    IDirect3DVertexBuffer9* d3dVertexBuffer;

public:
    D3D9ExVertexBuffer(D3D9ExDevice* d3dDevice, IDirect3DVertexBuffer9* d3dVertexBuffer);

    virtual HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags);
    virtual HRESULT Unlock();
    virtual HRESULT GetDesc(D3DVERTEXBUFFER_DESC* pDesc);

    IDirect3DVertexBuffer9* GetD3DVertexBuffer() const
    {
        return d3dVertexBuffer;
    }
};
