#pragma once

#include <d3d11.h>

namespace GenerationsD3D11
{
    inline ID3D11Device* (*GetDevice)(void* dxpDevice);
    inline ID3D11DeviceContext* (*GetDeviceContext)(void* dxpDevice);

    inline void(*LockDevice)(void* dxpDevice);
    inline void(*UnlockDevice)(void* dxpDevice);

    inline ID3D11Resource* (*GetResource)(void* dxpResource);

    class LockGuard
    {
    protected:
        void* dxpDevice;

    public:
        LockGuard(void* dxpDevice) : dxpDevice(dxpDevice)
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

        if (any)
        {
            MessageBox(nullptr, TEXT("Failed to initialize functions for GenerationsD3D11 API!"), TEXT("GenerationsD3D11"), MB_ICONERROR);
            exit(-1);
        }
    }
}