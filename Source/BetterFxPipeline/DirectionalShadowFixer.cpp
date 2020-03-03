#include "Configuration.h"
#include "DirectionalShadowFixer.h"

double one = 1.0;

uint32_t renderGameSceneMidAsmHookDisableLambertReturnAddress = 0x10C7E3A;
uint32_t renderGameSceneMidAsmHookEnableLambertReturnAddress = 0x10C8049;

void __declspec(naked) renderGameSceneIsTerrain()
{
    __asm
    {
        push eax
        push ebx

        mov eax, [ebp + 0x08]
        mov eax, [eax]

        mov ebx, [ebp + 0x0C]
        mov ebx, [ebx]

        cmp eax, ebx

        pop ebx
        pop eax

        retn
    }
}

void __declspec(naked) renderGameSceneMidAsmHook()
{
    __asm
    {
        mov edx, 0x1A579A0
        movaps xmm0, [edx]
        movaps[esp + 0x20], xmm0
        xorps xmm1, xmm1

        call renderGameSceneIsTerrain
        jz loadTerrainBias

        mov edx, 0x1E5E32C // Ambient_Bias_Shadow_Object
        movss xmm0, [edx]
        jmp main

    loadTerrainBias:
        mov edx, 0x1E5E328 // Ambient_Shadow_Bias_Terrain
        movss xmm0, [edx]

    main:
        movsd xmm1, one
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
        call renderGameSceneIsTerrain
        jz moveIsEnableLambertShadowTerrain

        mov edx, 0x1A4358A // ms_IsEnableLambertShadowObject
        mov al, byte ptr[edx]
        jmp compareIsEnable

    moveIsEnableLambertShadowTerrain:
        mov edx, 0x1A43589 // ms_IsEnableLambertShadowTerrain
        mov al, byte ptr[edx]

    compareIsEnable:
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

bool DirectionalShadowFixer::enabled = false;

void DirectionalShadowFixer::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    WRITE_JUMP(0x10C7E35, renderGameSceneMidAsmHook);
}
