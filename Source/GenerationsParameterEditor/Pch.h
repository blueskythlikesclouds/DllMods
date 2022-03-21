#pragma once

#define WIN32_LEAN_AND_MEAN

#include <BlueBlur.h>

#include <Windows.h>
#include <detours.h>

#include <d3d11.h>
#include <d3d9.h>

#include <cstdint>
#include <cstdio>
#include <mutex>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

#include <INIReader.h>

#include <Helpers.h>

class CriticalSection
{
public:
	CRITICAL_SECTION criticalSection;

	CriticalSection()
	{
		InitializeCriticalSection(&criticalSection);
	}

	~CriticalSection()
	{
		DeleteCriticalSection(&criticalSection);
	}

	void lock()
	{
		EnterCriticalSection(&criticalSection);
	}

	void unlock()
	{
		LeaveCriticalSection(&criticalSection);
	}
};