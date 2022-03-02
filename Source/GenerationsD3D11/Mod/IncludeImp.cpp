#include "Include/GenerationsD3D11.h"

#include "Device.h"
#include "Resource.h"

extern "C" __declspec(dllexport) ID3D11Device* GenerationsD3D11_GetDevice(void* dxpDevice)
{
    return reinterpret_cast<Device*>(dxpDevice)->get();
}

extern "C" __declspec(dllexport) ID3D11DeviceContext* GenerationsD3D11_GetDeviceContext(void* dxpDevice)
{
    return reinterpret_cast<Device*>(dxpDevice)->getContext();
}

extern "C" __declspec(dllexport) void GenerationsD3D11_LockDevice(void* dxpDevice)
{
    reinterpret_cast<Device*>(dxpDevice)->getCriticalSection().lock();
}

extern "C" __declspec(dllexport) void GenerationsD3D11_UnlockDevice(void* dxpDevice)
{
    reinterpret_cast<Device*>(dxpDevice)->getCriticalSection().unlock();
}

extern "C" __declspec(dllexport) ID3D11Resource* GenerationsD3D11_GetResource(void* dxpDevice)
{
    return reinterpret_cast<Resource*>(dxpDevice)->getResource();
}