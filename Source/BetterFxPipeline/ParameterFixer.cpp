#include "ParameterFixer.h"

HOOK(void*, __fastcall, SetShader, 0x415EE0, YggSchedulerSubInternal* This, void* Edx, const boost::shared_ptr<VertexShaderData>& vertexShaderData, const boost::shared_ptr<PixelShaderData>& pixelShaderData)
{
    void* result = originalSetShader(This, Edx, vertexShaderData, pixelShaderData);

    // g_ForceAlphaColor
    const float forceAlphaColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const uint8_t forceAlphaColorRegister = *(uint8_t*)((uint8_t*)This->renderingInfrastructure->internal.globalParametersData + 
        This->renderingInfrastructure->internal.globalParameterIndex * 2 + 544);

    if (forceAlphaColorRegister != 255)
        This->dxpDevice->SetPixelShaderConstantF(forceAlphaColorRegister, forceAlphaColor, 1);

    return result;
}

FUNCTION_PTR(void*, __thiscall, resetForceAlphaColor, 0x10D4EB0, void* This);

HOOK(void*, __fastcall, RenderMeshes, 0x789890, void* This, void* Edx, void* a2, void* a3)
{
    resetForceAlphaColor(This);
    return originalRenderMeshes(This, Edx, a2, a3);
}

bool ParameterFixer::enabled = false;

void ParameterFixer::applyPatches()
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
