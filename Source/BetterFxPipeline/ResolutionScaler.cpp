#include "Configuration.h"
#include "ResolutionScaler.h"

void setScales(uint32_t& This, float& widthScale, float& heightScale)
{
    uint32_t width = *(uint32_t*)(This + 200);
    uint32_t height = *(uint32_t*)(This + 204);

    if (ResolutionScaler::scaleTo720p)
    {
        widthScale = 1280.0f / width * widthScale;
        heightScale = 720.0f / height * heightScale;
    }
    else if (Configuration::width > 0 && Configuration::height > 0)
    {
        widthScale *= static_cast<float>(Configuration::width) / static_cast<float>(width);
        heightScale *= static_cast<float>(Configuration::height) / static_cast<float>(height);
    }
}

HOOK(uint32_t, __fastcall, CreateFrameBuffer1, 0x787980,
     uint32_t This, uint32_t Edx, uint32_t a2, uint32_t a3, float widthScale, float heightScale, uint32_t a6,
     uint32_t a7)
{
    setScales(This, widthScale, heightScale);
    return originalCreateFrameBuffer1(This, Edx, a2, a3, widthScale, heightScale, a6, a7);
}

HOOK(uint32_t, __fastcall, CreateFrameBuffer2, 0x787A30,
     uint32_t This, uint32_t Edx, uint32_t a2, float widthScale, float heightScale, uint32_t a5, uint32_t a6,
     uint32_t a7)
{
    setScales(This, widthScale, heightScale);
    return originalCreateFrameBuffer2(This, Edx, a2, widthScale, heightScale, a5, a6, a7);
}

HOOK(uint32_t, __fastcall, CreateFrameBuffer3, 0x7884C0,
     uint32_t This, uint32_t Edx, uint32_t a2, float widthScale, float heightScale, uint32_t a5, uint32_t a6,
     uint32_t a7, uint32_t a8, uint32_t a9)
{
    setScales(This, widthScale, heightScale);
    return originalCreateFrameBuffer3(This, Edx, a2, widthScale, heightScale, a5, a6, a7, a8, a9);
}

HOOK(uint32_t, __fastcall, CreateFrameBuffer4, 0x788590,
     uint32_t This, uint32_t Edx, uint32_t a2, float widthScale, float heightScale, uint32_t a5, uint32_t a6,
     uint32_t a7, uint32_t a8, uint32_t a9)
{
    setScales(This, widthScale, heightScale);
    return originalCreateFrameBuffer4(This, Edx, a2, widthScale, heightScale, a5, a6, a7, a8, a9);
}

bool ResolutionScaler::enabled = false;
bool ResolutionScaler::scaleTo720p = false;

void ResolutionScaler::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    INSTALL_HOOK(CreateFrameBuffer1);
    INSTALL_HOOK(CreateFrameBuffer2);
    INSTALL_HOOK(CreateFrameBuffer3);
    INSTALL_HOOK(CreateFrameBuffer4);
}
