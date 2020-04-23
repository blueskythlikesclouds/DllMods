#pragma once

#include "D3D9ExUnknown.h"

class D3D9ExDevice;

class D3D9ExPixelShader : public D3D9ExUnknown
{
    D3D9ExDevice* dxpDevice;
    IDirect3DPixelShader9* d3dPixelShader;

public:
    D3D9ExPixelShader(D3D9ExDevice* dxpDevice, IDirect3DPixelShader9* d3dPixelShader);
    ~D3D9ExPixelShader();

    virtual HRESULT GetDevice(D3D9ExDevice** ppDevice);
    virtual HRESULT GetFunction(void*, UINT* pSizeOfData);

    IDirect3DPixelShader9* GetD3DPixelShader() const
    {
        return d3dPixelShader;
    }
};
