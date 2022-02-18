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
        PRINT("GenerationsD3D11: %s() stubbed\n", #name); \
        return (returnType)E_FAIL; \
    }

#include <Windows.h>
#include <detours.h>
#include <wrl/client.h>

#include <d3d11.h>
#include <d3d12.h>
#include <d3d9.h>
#include <dxgi1_4.h>

#include <DDSTextureLoader11.h>

#include <bitset>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <mutex>
#include <vector>

#include <BlueBlur.h>
#include <Helpers.h>
#include <INIReader.h>
#include <LostCodeLoader.h>
#include "../../BetterFxPipeline/Hash.h"

using Microsoft::WRL::ComPtr;