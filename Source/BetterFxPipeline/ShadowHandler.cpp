#include "ShadowHandler.h"
#include "Configuration.h"

//
// FxPipeline
//

const volatile double ONE = 1.0;

uint32_t renderGameSceneMidAsmHookDisableLambertReturnAddress = 0x10C7E3A;
uint32_t renderGameSceneMidAsmHookEnableLambertReturnAddress = 0x10C8049;

void __declspec(naked) renderGameSceneMidAsmHook()
{
    __asm
    {
        mov edx, 0x1A579A0
        movaps xmm0, [edx]
        movaps[esp + 0x20], xmm0
        xorps xmm1, xmm1

        mov eax, [ebp + 0x08]
        mov eax, [eax]

        mov ebx, [ebp + 0x0C]
        mov ebx, [ebx]

        cmp eax, ebx
        jz loadTerrainBias

        mov edx, 0x1E5E32C // Ambient_Shadow_Bias_Object
        jmp main

    loadTerrainBias:
        mov edx, 0x1E5E328 // Ambient_Shadow_Bias_Terrain

    main:
        movss xmm0, [edx]
        movsd xmm1, ONE
        cvtps2pd xmm0, xmm0
        subsd xmm1, xmm0
        cvtpd2ps xmm0, xmm1
        movss[esp + 0x2C], xmm0

        mov eax, [edi]
        mov ecx, [eax + 8]
        movzx edx, byte ptr[ecx + 0x23A]
        cmp edx, 0xFF
        jz return
        mov ecx, [eax + 4]
        mov eax, [ecx]
        push 1
        lea esi, [esp + 0x24]
        push esi
        push edx
        mov edx, [eax + 0x1B8]
        call edx

    return:
        mov eax, [ebp + 0x08]
        mov eax, [eax]

        mov ebx, [ebp + 0x0C]
        mov ebx, [ebx]

        cmp eax, ebx
        jz moveIsEnableLambertShadowTerrain

        mov edx, 0x1A4358A // ms_IsEnableLambertShadowObject
        jmp compareIsEnable

    moveIsEnableLambertShadowTerrain:
        mov edx, 0x1A43589 // ms_IsEnableLambertShadowTerrain

    compareIsEnable:
        mov al, byte ptr[edx]
        cmp al, 1
        jz enableLambert

        mov edi, [edi]
        mov edx, [edi + 8]
        jmp[renderGameSceneMidAsmHookDisableLambertReturnAddress]

    enableLambert:
        mov edi, [edi]
        mov edx, [edi + 8]
        jmp[renderGameSceneMidAsmHookEnableLambertReturnAddress]
    }
}

//
// MTFx
//

bool* IS_ENABLE_VERTICAL_SHADOW = (bool*)0x1A43588; // ms_IsEnableVerticalShadow
uint32_t* SHADOW_MAP_NO_TERRAIN_CAMERA_TYPE = (uint32_t*)0x13DD29C;

uint32_t setVerticalLightViewProjectionMidAsmHookReturnAddress = 0x650F37;

void __declspec(naked) setVerticalLightViewProjectionMidAsmHook()
{
    __asm
    {
        mov eax, IS_ENABLE_VERTICAL_SHADOW
        mov al, byte ptr[eax]

        cmp al, 0
        jz enableDirectionalShadow

        // Enable vertical shadow
        imul ecx, 0x1F0
        jmp end

    enableDirectionalShadow:
        mov ecx, 0x3E0

    end:
        jmp[setVerticalLightViewProjectionMidAsmHookReturnAddress]
    }
}

uint32_t setLightViewProjectionMidAsmHookReturnAddress = 0x650FB5;

void __declspec(naked) setLightViewProjectionMidAsmHook()
{
    __asm
    {
        mov eax, IS_ENABLE_VERTICAL_SHADOW
        mov al, byte ptr[eax]

        cmp al, 0
        jz enableDirectionalShadow

        // Enable vertical shadow
        imul ebx, 0x1F0
        jmp end

    enableDirectionalShadow:
        mov ebx, 0x3E0

    end:
        jmp[setLightViewProjectionMidAsmHookReturnAddress]
    }
}

HOOK(void, __stdcall, MTFxSetTargetSurfaceResolutions, 0x653A00, void* A1, void* A2, void* A3)
{
    originalMTFxSetTargetSurfaceResolutions(A1, A2, A3);

    *(uint32_t*)((char*)A2 + 32) = Configuration::shadowResolution;
    *(uint32_t*)((char*)A2 + 36) = Configuration::shadowResolution;
}

bool ShadowHandler::enabled = false;

void ShadowHandler::update()
{
    if (!enabled) return;

    *SHADOW_MAP_NO_TERRAIN_CAMERA_TYPE = *IS_ENABLE_VERTICAL_SHADOW ? 3 : 2;
}

void ShadowHandler::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    // FxPipeline
    WRITE_JUMP(0x10C7E35, renderGameSceneMidAsmHook);
    WRITE_MEMORY(0x10C6039, uint32_t, Configuration::shadowResolution);

    // MTFx
    WRITE_JUMP(0x650F31, setVerticalLightViewProjectionMidAsmHook);
    WRITE_JUMP(0x650FAF, setLightViewProjectionMidAsmHook);

    DWORD oldProtect;
    VirtualProtect(SHADOW_MAP_NO_TERRAIN_CAMERA_TYPE, sizeof(uint32_t), PAGE_READWRITE, &oldProtect);

    INSTALL_HOOK(MTFxSetTargetSurfaceResolutions);
}
