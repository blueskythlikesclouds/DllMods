#include "Configuration.h"
#include "ResolutionScaler.h"

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

bool ResolutionScaler::enabled = false;
bool ResolutionScaler::scaleTo720p = false;

void ResolutionScaler::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    INSTALL_HOOK(CreateYggTexture1);
    INSTALL_HOOK(CreateYggTexture2);
    INSTALL_HOOK(CreateYggTexture3);
    INSTALL_HOOK(CreateYggTexture4);
}
