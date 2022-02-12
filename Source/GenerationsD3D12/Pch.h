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
        return (returnType)E_FAIL; \
    }

#include <Windows.h>
#include <detours.h>
#include <wrl/client.h>

#include <d3d9.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_4.h>

#include <DDSTextureLoader12.h>
#include <D3D12MemAlloc.h>

#include <cstdint>
#include <cstdio>
#include <vector>
#include <memory>
#include <mutex>

#include <INIReader.h>
#include <Helpers.h>
#include <BlueBlur.h>
#include "../BetterFxPipeline/Hash.h"

using Microsoft::WRL::ComPtr;