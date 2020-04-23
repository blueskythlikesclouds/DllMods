#include "D3D9ExPixelShader.h"
#include "D3D9ExDevice.h"

D3D9ExPixelShader::D3D9ExPixelShader(D3D9ExDevice* dxpDevice, IDirect3DPixelShader9* d3dPixelShader) : dxpDevice(dxpDevice), d3dPixelShader(d3dPixelShader)
{
    dxpDevice->AddRef();
    d3dPixelShader->AddRef();
}

D3D9ExPixelShader::~D3D9ExPixelShader()
{
    dxpDevice->Release();
    d3dPixelShader->Release();
}

HRESULT D3D9ExPixelShader::GetDevice(D3D9ExDevice** ppDevice)
{
    *ppDevice = dxpDevice;
    (*ppDevice)->AddRef();

    return S_OK;
}

HRESULT D3D9ExPixelShader::GetFunction(void* A1, UINT* pSizeOfData)
{
    return d3dPixelShader->GetFunction(A1, pSizeOfData);
}