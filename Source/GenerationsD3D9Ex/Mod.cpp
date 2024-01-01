#include "Configuration.h"
#include "D3D9ExHandler.h"
#include "LostCodeLoader.h"
#include "DDSHandler.h"
#include "MemoryHandler.h"

HICON __stdcall LoadIconImpl(HINSTANCE hInstance, LPCSTR lpIconName)
{
    return LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(2057));
}

extern "C" __declspec(dllexport) void Init(ModInfo* info)
{
    MemoryHandler::applyPatches();

    std::string dir = info->CurrentMod->Path;

    size_t pos = dir.find_last_of("\\/");
    if (pos != std::string::npos)
        dir.erase(pos + 1);

    if (!Configuration::load(dir + "GenerationsD3D9Ex.ini"))
        MessageBox(NULL, L"Failed to parse GenerationsD3D9Ex.ini", NULL, MB_ICONERROR);

    D3D9ExHandler::applyPatches();
    DDSHandler::applyPatches();

    // Hide window when it's first created because it's not a pleasant sight to see it centered/resized afterwards.
    if (Configuration::displayMode != DisplayMode::FULLSCREEN)
        WRITE_MEMORY(0xE7B8F7, uint8_t, 0x00);

    // Patch the window function to load the icon in the executable.
    // However, check whether any mods already wrote over this first.
    if (*reinterpret_cast<uint8_t*>(0xE7B843) == 0xFF)
    {
        WRITE_CALL(0xE7B843, LoadIconImpl);
        WRITE_NOP(0xE7B848, 1);
    }
}
