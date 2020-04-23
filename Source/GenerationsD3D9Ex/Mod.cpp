#include "D3D9ExHandler.h"
#include "TextureHandler.h"

extern "C" __declspec(dllexport) void Init()
{
    D3D9ExHandler::applyPatches();
    TextureHandler::applyPatches();
}
