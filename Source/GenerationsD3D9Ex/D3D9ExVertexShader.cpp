#include "D3D9ExVertexShader.h"
#include "D3D9ExDevice.h"

D3D9ExVertexShader::D3D9ExVertexShader(D3D9ExDevice* dxpDevice, IDirect3DVertexShader9* d3dVertexShader) : dxpDevice(dxpDevice), d3dVertexShader(d3dVertexShader)
{
    dxpDevice->AddRef();
    d3dVertexShader->AddRef();
}

D3D9ExVertexShader::~D3D9ExVertexShader()
{
    dxpDevice->Release();
    d3dVertexShader->Release();
}

HRESULT D3D9ExVertexShader::GetDevice(D3D9ExDevice** ppDevice)
{
    *ppDevice = dxpDevice;
    (*ppDevice)->AddRef();

    return S_OK;
}

HRESULT D3D9ExVertexShader::GetFunction(void* A1, UINT* pSizeOfData)
{
    return d3dVertexShader->GetFunction(A1, pSizeOfData);
}