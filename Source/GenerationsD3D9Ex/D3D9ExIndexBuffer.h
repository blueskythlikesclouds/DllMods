#pragma once

#include "D3D9ExResource.h"

class D3D9ExIndexBuffer : public D3D9ExResource
{
    IDirect3DIndexBuffer9* d3dIndexBuffer;

public:
    D3D9ExIndexBuffer(D3D9ExDevice* d3dDevice, IDirect3DIndexBuffer9* d3dIndexBuffer);

    virtual HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags) final;
    virtual HRESULT Unlock() final;
    virtual HRESULT GetDesc(D3DINDEXBUFFER_DESC* pDesc) final;

    IDirect3DIndexBuffer9* GetD3DIndexBuffer() const
    {
        return d3dIndexBuffer;
    }
};