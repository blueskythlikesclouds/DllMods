#include "ParticlePostProcessor.h"

uint32_t renderParticles;
uint32_t renderParticleMidAsmHookReturnAddress = 0x10C8278;
uint32_t renderParticleMidAsmHookFunctionAddress = 0x789890;

void __declspec(naked) renderParticleMidAsmHook()
{
    __asm
    {
        cmp renderParticles, 1
        je atTrue

        add esp, 8
        jmp[renderParticleMidAsmHookReturnAddress]

    atTrue:
        call[renderParticleMidAsmHookFunctionAddress]
        jmp[renderParticleMidAsmHookReturnAddress]
    }
}

uint32_t executeRenderJobsMidAsmHookOriginalFunctionAddress = 0x78A2E0;

void __declspec(naked) executeRenderJobsMidAsmHookOriginal()
{
    __asm
    {
        mov eax, [edi]
        mov[esi + 0x30], eax
        mov ebx, [edi + 4]
        cmp ebx, [esi + 0x34]
        jz loc_78A45A
        test ebx, ebx
        jz loc_78A43D
        lea ecx, [ebx + 8]
        mov edx, 1
        lock xadd[ecx], edx

    loc_78A43D:
        mov ecx, [esi + 0x34]
        test ecx, ecx
        jz loc_78A457
        lea eax, [ecx + 8]
        or edx, 0xFFFFFFFF
        lock xadd[eax], edx
        jnz loc_78A457
        mov eax, [ecx]
        mov edx, [eax + 8]
        call edx

    loc_78A457:
        mov[esi + 0x34], ebx

    loc_78A45A:
        mov eax, [edi]
        push eax
        mov ecx, esi
        call[executeRenderJobsMidAsmHookOriginalFunctionAddress]

        retn
    }
}

uint32_t executeRenderJobsMidAsmHookReturnAddress = 0x78A425;

void __declspec(naked) executeRenderJobsMidAsmHook()
{
    __asm
    {
        push edi

        sub edi, [esi + 36]

        // Before CFxRenderGameScene
        cmp edi, 40
        jz after

        // At CFxRenderParticle
        cmp edi, 72
        jz at

        jmp return

    after:
        mov renderParticles, 1
        mov edi, [esi + 36]
        add edi, 72
        call executeRenderJobsMidAsmHookOriginal
        jmp return

    at:
        mov renderParticles, 0

    return:
        pop edi
        mov eax, [edi]
        mov[esi + 0x30], eax
        jmp[executeRenderJobsMidAsmHookReturnAddress]
    }
}

bool ParticlePostProcessor::enabled = false;

void ParticlePostProcessor::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    WRITE_JUMP(0x10C8273, renderParticleMidAsmHook);
    WRITE_JUMP(0x78A420, executeRenderJobsMidAsmHook);
}
