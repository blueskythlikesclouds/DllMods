#pragma once

#include <d3d11.h>

namespace DX_PATCH 
{
    class IDirect3DBaseTexture9;
    class IDirect3DDevice9;
    class IDirect3DResource9;
}

namespace GenerationsD3D11
{
    inline ID3D11Device* (*GetDevice)(DX_PATCH::IDirect3DDevice9* dxpDevice);
    inline ID3D11DeviceContext* (*GetDeviceContext)(DX_PATCH::IDirect3DDevice9* dxpDevice);

    inline void(*LockDevice)(DX_PATCH::IDirect3DDevice9* dxpDevice);
    inline void(*UnlockDevice)(DX_PATCH::IDirect3DDevice9* dxpDevice);

    inline ID3D11Resource* (*GetResource)(DX_PATCH::IDirect3DResource9* dxpResource);
    inline ID3D11ShaderResourceView* (*GetShaderResourceView)(DX_PATCH::IDirect3DBaseTexture9* dxpBaseTexture);
    inline ID3D11RenderTargetView* (*GetRenderTargetView)(DX_PATCH::IDirect3DBaseTexture9* dxpBaseTexture);
    inline ID3D11DepthStencilView* (*GetDepthStencilView)(DX_PATCH::IDirect3DBaseTexture9* dxpBaseTexture);

    class LockGuard
    {
    protected:
        DX_PATCH::IDirect3DDevice9* dxpDevice;

    public:
        LockGuard(DX_PATCH::IDirect3DDevice9* dxpDevice) : dxpDevice(dxpDevice)
        {
            if (dxpDevice)
                LockDevice(dxpDevice);
        }

        ~LockGuard()
        {
            if (dxpDevice)
                UnlockDevice(dxpDevice);
        }
    };

    template<typename T>
    inline bool SetFunctionPointer(T*& dest, HMODULE module, LPCSTR procName)
    {
        dest = reinterpret_cast<T*>(GetProcAddress(module, procName));
        return dest == nullptr;
    }

    inline void SetFunctionPointers()
    {
        HMODULE module = GetModuleHandle(TEXT("GenerationsD3D11.dll"));

        bool any = module == nullptr;
        any |= SetFunctionPointer(GetDevice, module, "GenerationsD3D11_GetDevice");
        any |= SetFunctionPointer(GetDeviceContext, module, "GenerationsD3D11_GetDeviceContext");
        any |= SetFunctionPointer(LockDevice, module, "GenerationsD3D11_LockDevice");
        any |= SetFunctionPointer(UnlockDevice, module, "GenerationsD3D11_UnlockDevice");
        any |= SetFunctionPointer(GetResource, module, "GenerationsD3D11_GetResource");
        any |= SetFunctionPointer(GetShaderResourceView, module, "GenerationsD3D11_GetShaderResourceView");
        any |= SetFunctionPointer(GetRenderTargetView, module, "GenerationsD3D11_GetRenderTargetView");
        any |= SetFunctionPointer(GetDepthStencilView, module, "GenerationsD3D11_GetDepthStencilView");

        if (any)
        {
            MessageBox(nullptr, TEXT("Failed to initialize functions for GenerationsD3D11 API!"), TEXT("GenerationsD3D11"), MB_ICONERROR);
            exit(-1);
        }
    }
}