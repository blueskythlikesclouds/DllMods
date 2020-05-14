#include "ConstantParameterFixer.h"

HOOK(uint32_t, __fastcall, SetShader, 0x415EE0, void* This, void* Edx, void* vertexShader, void* pixelShader)
{
    const uint32_t result = originalSetShader(This, Edx, vertexShader, pixelShader);

    const uint32_t field60 = *static_cast<uint32_t*>(This) + 0x60;
    DX_PATCH::IDirect3DDevice9* dxpDevice = *(DX_PATCH::IDirect3DDevice9**)(field60 + 4);
    const uint32_t parameters = *(uint32_t*)(field60 + 8);

    // g_ForceAlphaColor
    const float forceAlphaColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const uint8_t forceAlphaColorRegister = *(uint8_t*)(parameters + *(uint32_t*)(field60 + 16) * 2 + 544);
    if (forceAlphaColorRegister != 255)
        dxpDevice->SetPixelShaderConstantF(forceAlphaColorRegister, forceAlphaColor, 1);

    return result;
}

FUNCTION_PTR(uint32_t, __thiscall, resetForceAlphaColor, 0x10D4EB0, void* This);

HOOK(uint32_t, __fastcall, RenderMeshes, 0x789890, void* This, void* Edx, uint32_t a2, uint32_t a3)
{
    resetForceAlphaColor(This);
    return originalRenderMeshes(This, Edx, a2, a3);
}

bool ConstantParameterFixer::enabled = false;

void ConstantParameterFixer::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    // Set g_ForceAlphaColor's default value to white instead of transparent red
    WRITE_MEMORY(0x10D4F62, uint8_t, 0x4C);
    WRITE_MEMORY(0x10D4F68, uint8_t, 0x4C);
    WRITE_MEMORY(0x10D4F6E, uint8_t, 0x4C);

    // Fix mrgPlayableParam's default values
    WRITE_MEMORY(0x16A0D44, float, -10000, 64);
    WRITE_MEMORY(0x16995E8, float, -10000, 64);

    // Initialize g_ForceAlphaColor for every object
    INSTALL_HOOK(SetShader);

    // Initialize g_ForceAlphaColor for every object before rendering
    // because the hook above can't always be trusted apparently
    INSTALL_HOOK(RenderMeshes);
}
