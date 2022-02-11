#pragma once

#define WIN32_LEAN_AND_MEAN

#if _DEBUG
#define PRINT(x, ...) printf(x, __VA_ARGS__);
#else
#define PRINT(x, ...)
#endif

#define FUNCTION_STUB(returnType, name, ...) \
    returnType name(__VA_ARGS__) \
    { \
        PRINT("GenerationsD3D12: %s() stubbed\n", #name); \
        return (returnType)NULL; \
    }

#include <Windows.h>
#include <detours.h>

#include <cstdint>
#include <cstdio>
#include <vector>
#include <memory>
#include <mutex>

#include <d3d9.h>
#include <d3d12.h>
#include <d3dx12.h>

#include <dxgi1_4.h>

#include <INIReader.h>

#include <Helpers.h>

#include <BlueBlur.h>

#include <DDSTextureLoader12.h>

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;