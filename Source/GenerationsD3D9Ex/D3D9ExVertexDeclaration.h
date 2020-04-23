#pragma once

#include "D3D9ExUnknown.h"

class D3D9ExDevice;

class D3D9ExVertexDeclaration : public D3D9ExUnknown
{
    D3D9ExDevice* dxpDevice;
    IDirect3DVertexDeclaration9* d3dVertexDeclaration;

public:
    D3D9ExVertexDeclaration(D3D9ExDevice* dxpDevice, IDirect3DVertexDeclaration9* d3dVertexDeclaration);
    ~D3D9ExVertexDeclaration();

    virtual HRESULT GetDevice(D3D9ExDevice** ppDevice);
    virtual HRESULT GetDeclaration(D3DVERTEXELEMENT9* pElement, UINT* pNumElements);

    IDirect3DVertexDeclaration9* GetD3DVertexDeclaration() const
    {
        return d3dVertexDeclaration;
    }
};
