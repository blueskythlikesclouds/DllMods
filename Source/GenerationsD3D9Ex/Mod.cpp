#include "Configuration.h"
#include "D3D9ExHandler.h"
#include "LostCodeLoader.h"
#include "TextureHandler.h"

extern "C" __declspec(dllexport) void Init(ModInfo* info)
{
    std::string dir = info->CurrentMod->Path;

    size_t pos = dir.find_last_of("\\/");
    if (pos != std::string::npos)
        dir.erase(pos + 1);

    if (!Configuration::load(dir + "GenerationsD3D9Ex.ini"))
        MessageBox(NULL, L"Failed to parse GenerationsD3D9Ex.ini", NULL, MB_ICONERROR);

    D3D9ExHandler::applyPatches();
    TextureHandler::applyPatches();

    // Hide window when it's first created because it's not a pleasant sight to see it centered/resized afterwards.
    if (Configuration::displayMode != DisplayMode::FULLSCREEN)
        WRITE_MEMORY(0xE7B8F7, uint8_t, 0x00);
}
