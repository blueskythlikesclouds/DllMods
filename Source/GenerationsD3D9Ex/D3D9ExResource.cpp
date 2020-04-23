#include "D3D9ExDevice.h"
#include "D3D9ExResource.h"

D3D9ExResource::D3D9ExResource(D3D9ExDevice* d3dDevice, IDirect3DResource9* d3dResource) : dxpDevice(d3dDevice), d3dResource(d3dResource)
{
    d3dDevice->AddRef();
    d3dResource->AddRef();
}

D3D9ExResource::~D3D9ExResource()
{
    dxpDevice->Release();
    d3dResource->Release();
}

HRESULT D3D9ExResource::GetDevice(D3D9ExDevice** ppDevice)
{
    *ppDevice = dxpDevice;
    (*ppDevice)->AddRef();

    return S_OK;
}

HRESULT D3D9ExResource::SetPrivateData(REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags)
{
    return d3dResource->SetPrivateData(refguid, pData, SizeOfData, Flags);
}

HRESULT D3D9ExResource::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
    return d3dResource->GetPrivateData(refguid, pData, pSizeOfData);
}

HRESULT D3D9ExResource::FreePrivateData(REFGUID refguid)
{
    return d3dResource->FreePrivateData(refguid);
}

DWORD D3D9ExResource::SetPriority(DWORD PriorityNew)
{
    return d3dResource->SetPriority(PriorityNew);
}

DWORD D3D9ExResource::GetPriority()
{
    return d3dResource->GetPriority();
}

void D3D9ExResource::PreLoad()
{
    return d3dResource->PreLoad();
}

D3DRESOURCETYPE D3D9ExResource::GetType()
{
    return d3dResource->GetType();
}