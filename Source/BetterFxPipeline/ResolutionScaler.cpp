#include "Configuration.h"
#include "ResolutionScaler.h"

// Shaders
#include "CopyTexture.wpu.h"
#include "CopyTexture.wvu.h"

void setScales(YggSchedulerSubInternal* subInternal, float& widthScale, float& heightScale)
{
    if (ResolutionScaler::scaleTo720p)
    {
        widthScale = 1280.0f / subInternal->width * widthScale;
        heightScale = 720.0f / subInternal->height * heightScale;
    }
    else if (Configuration::width > 0 && Configuration::height > 0)
    {
        widthScale *= (float)Configuration::width / (float)subInternal->width;
        heightScale *= (float)Configuration::height / (float)subInternal->height;
    }
}

HOOK(void*, __fastcall, CreateYggTexture1, 0x787980, 
    YggSchedulerSubInternal* This, uint32_t Edx, uint32_t a2, uint32_t a3, float widthScale, float heightScale, uint32_t a6, uint32_t a7)
{
    setScales(This, widthScale, heightScale);
    return originalCreateYggTexture1(This, Edx, a2, a3, widthScale, heightScale, a6, a7);
}

HOOK(void*, __fastcall, CreateYggTexture2, 0x787A30, 
    YggSchedulerSubInternal* This, uint32_t Edx, uint32_t a2, float widthScale, float heightScale, uint32_t a5, uint32_t a6, uint32_t a7)
{
    setScales(This, widthScale, heightScale);
    return originalCreateYggTexture2(This, Edx, a2, widthScale, heightScale, a5, a6, a7);
}

HOOK(void*, __fastcall, CreateYggTexture3, 0x7884C0, 
    YggSchedulerSubInternal* This, uint32_t Edx, uint32_t a2, float widthScale, float heightScale, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9)
{
    setScales(This, widthScale, heightScale);
    return originalCreateYggTexture3(This, Edx, a2, widthScale, heightScale, a5, a6, a7, a8, a9);
}

HOOK(void*, __fastcall, CreateYggTexture4, 0x788590, 
    YggSchedulerSubInternal* This, uint32_t Edx, uint32_t a2, float widthScale, float heightScale, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9)
{
    setScales(This, widthScale, heightScale);
    return originalCreateYggTexture4(This, Edx, a2, widthScale, heightScale, a5, a6, a7, a8, a9);
}

HOOK(void, __stdcall, InitializeSurfaceInfo, 0x653A00, void* A1, void* A2, void* A3)
{
    originalInitializeSurfaceInfo(A1, A2, A3);

    if (!Configuration::enableResolutionScale)
        return;

    *(uint32_t*)((char*)A2 + 16) = Configuration::width;
    *(uint32_t*)((char*)A2 + 20) = Configuration::height;
    *(uint32_t*)((char*)A2 + 24) = Configuration::width;
    *(uint32_t*)((char*)A2 + 28) = Configuration::height;
}

bool ResolutionScaler::enabled = false;
bool ResolutionScaler::scaleTo720p = false;

void ResolutionScaler::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    // FxPipeline
    INSTALL_HOOK(CreateYggTexture1);
    INSTALL_HOOK(CreateYggTexture2);
    INSTALL_HOOK(CreateYggTexture3);
    INSTALL_HOOK(CreateYggTexture4);

    // MTFx
    INSTALL_HOOK(InitializeSurfaceInfo);

    // Replace Devil's Details' shaders
    WRITE_MEMORY(0x64C88D, uint32_t, _countof(g_ps30_main));
    WRITE_MEMORY(0x64C897, void*, g_ps30_main);

    WRITE_MEMORY(0x64C855, uint32_t, _countof(g_vs30_main));
    WRITE_MEMORY(0x64C85F, void*, g_vs30_main);

    WRITE_MEMORY(0x64CBF7, uint32_t, 440); // SetPixelShaderConstantF
}
