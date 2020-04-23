#pragma once

#include "D3D9ExUnknown.h"

class D3D9ExDevice;

class D3D9ExVertexShader : public D3D9ExUnknown
{
    D3D9ExDevice* dxpDevice;
    IDirect3DVertexShader9* d3dVertexShader;

public:
    D3D9ExVertexShader(D3D9ExDevice* dxpDevice, IDirect3DVertexShader9* d3dVertexShader);
    ~D3D9ExVertexShader();

    virtual HRESULT GetDevice(D3D9ExDevice** ppDevice);
    virtual HRESULT GetFunction(void*, UINT* pSizeOfData);

    IDirect3DVertexShader9* GetD3DVertexShader() const
    {
        return d3dVertexShader;
    }
};
