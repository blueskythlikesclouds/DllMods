#include "D3D9ExVertexDeclaration.h"
#include "D3D9ExDevice.h"

D3D9ExVertexDeclaration::D3D9ExVertexDeclaration(D3D9ExDevice* dxpDevice,
    IDirect3DVertexDeclaration9* d3dVertexDeclaration)
    : dxpDevice(dxpDevice), d3dVertexDeclaration(d3dVertexDeclaration)
{
    dxpDevice->AddRef();
    d3dVertexDeclaration->AddRef();
}

D3D9ExVertexDeclaration::~D3D9ExVertexDeclaration()
{
    dxpDevice->Release();
    d3dVertexDeclaration->Release();
}

HRESULT D3D9ExVertexDeclaration::GetDevice(D3D9ExDevice** ppDevice)
{
    *ppDevice = dxpDevice;
    (*ppDevice)->AddRef();

    return S_OK;
}

HRESULT D3D9ExVertexDeclaration::GetDeclaration(D3DVERTEXELEMENT9* pElement, UINT* pNumElements)
{
    return d3dVertexDeclaration->GetDeclaration(pElement, pNumElements);
}