#include "Include/GenerationsD3D11.h"

#include "DepthStencilTexture.h"
#include "Device.h"
#include "RenderTargetTexture.h"
#include "Resource.h"
#include "Texture.h"

extern "C" __declspec(dllexport) ID3D11Device* GenerationsD3D11_GetDevice(DX_PATCH::IDirect3DDevice9* dxpDevice)
{
    return reinterpret_cast<Device*>(dxpDevice)->get();
}

extern "C" __declspec(dllexport) ID3D11DeviceContext* GenerationsD3D11_GetDeviceContext(DX_PATCH::IDirect3DDevice9* dxpDevice)
{
    return reinterpret_cast<Device*>(dxpDevice)->getContext();
}

extern "C" __declspec(dllexport) void GenerationsD3D11_LockDevice(DX_PATCH::IDirect3DDevice9* dxpDevice)
{
    reinterpret_cast<Device*>(dxpDevice)->getCriticalSection().lock();
}

extern "C" __declspec(dllexport) void GenerationsD3D11_UnlockDevice(DX_PATCH::IDirect3DDevice9* dxpDevice)
{
    reinterpret_cast<Device*>(dxpDevice)->getCriticalSection().unlock();
}

extern "C" __declspec(dllexport) ID3D11Resource* GenerationsD3D11_GetResource(DX_PATCH::IDirect3DDevice9* dxpDevice)
{
    return reinterpret_cast<Resource*>(dxpDevice)->getResource();
}

extern "C" __declspec(dllexport) ID3D11ShaderResourceView* GenerationsD3D11_GetShaderResourceView(DX_PATCH::IDirect3DBaseTexture9* dxpBaseTexture)
{
    return reinterpret_cast<Texture*>(dxpBaseTexture)->getSRV();
}

extern "C" __declspec(dllexport) ID3D11RenderTargetView* GenerationsD3D11_GetRenderTargetView(DX_PATCH::IDirect3DBaseTexture9* dxpBaseTexture)
{
    return reinterpret_cast<RenderTargetTexture*>(dxpBaseTexture)->getRTV();
}

extern "C" __declspec(dllexport) ID3D11DepthStencilView* GenerationsD3D11_GetDepthStencilView(DX_PATCH::IDirect3DBaseTexture9* dxpBaseTexture)
{
    return reinterpret_cast<DepthStencilTexture*>(dxpBaseTexture)->getDSV();
}