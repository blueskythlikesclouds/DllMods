#include "ForceAlphaColorFixer.h"

HOOK(uint32_t, __fastcall, SetShader, 0x415EE0, void* This, void* Edx, void* vertexShader, void* pixelShader)
{
    const uint32_t field60 = *static_cast<uint32_t*>(This) + 0x60;
    const uint32_t device = *(uint32_t*)(field60 + 4);
    const uint32_t vtable = *(uint32_t*)device;
    const uint32_t parameters = *(uint32_t*)(field60 + 8);

    // g_ForceAlphaColor
    const float forceAlphaColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const uint8_t forceAlphaColorRegister = *(uint8_t*)(parameters + *(uint32_t*)(field60 + 16) * 2 + 544);
    if (forceAlphaColorRegister != 255)
        (*(HRESULT(__thiscall**)(uint32_t, UINT, const float*, UINT))(vtable + 440))(
            device, forceAlphaColorRegister, forceAlphaColor, 1); // SetPixelShaderConstantF

    return originalSetShader(This, Edx, vertexShader, pixelShader);
}

bool ForceAlphaColorFixer::enabled = false;

void ForceAlphaColorFixer::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    // Set g_ForceAlphaColor to white instead of transparent red
    WRITE_MEMORY(0x10D4F62, uint8_t, 0x4C);
    WRITE_MEMORY(0x10D4F68, uint8_t, 0x4C);
    WRITE_MEMORY(0x10D4F6E, uint8_t, 0x4C);

    // Initialize g_ForceAlphaColor for every object
    INSTALL_HOOK(SetShader);
}
