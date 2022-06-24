#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <detours.h>

#include <wrl/client.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#include <cstdint>
#include <cstdio>
#include <memory>

#include <Helpers.h>
#include <INIReader.h>
#include <xxhash.h>

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                                   \
                ((DWORD)(BYTE)(ch0)        | ((DWORD)(BYTE)(ch1) << 8) | \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif

using Microsoft::WRL::ComPtr;
