#include "BloomScaleFixer.h"
#include "ResolutionScaler.h"

//
// FxPipeline
//

HOOK(void*, __stdcall, InitializeBloomGlareFrameBuffers, 0x10D1C90, Sonic::CFxJob* This)
{
    ResolutionScaler::scaleTo720p = true;

    void* result = originalInitializeBloomGlareFrameBuffers(This);

    ResolutionScaler::scaleTo720p = false;

    return result;
}

//
// MTFx
//

uint32_t initializeTargetSurfaceColorD4LastMidAsmHookReturnAddress = 0x6557E3;

void __declspec(naked) initializeTargetSurfaceColorD4LastMidAsmHook()
{
    __asm
    {
        // 3D mode
        push eax
        mov eax, dword ptr[esp + 0x37C + 0x4 + 0x4]
        mov eax, dword ptr[eax + 0x74]
        test eax, eax
        pop eax
        jnz half

        mov dword ptr [esp + 0x37C - 0x1D0], 1280
        jmp end

    half:
        mov dword ptr[esp + 0x37C - 0x1D0], 640

    end:
        mov dword ptr [esp + 0x37C - 0x1CC], 720
        jmp[initializeTargetSurfaceColorD4LastMidAsmHookReturnAddress]
    }
}

uint32_t initializeTargetSurfaceColorD8LastMidAsmHookReturnAddress = 0x65581C;

void __declspec(naked) initializeTargetSurfaceColorD8LastMidAsmHook()
{
    __asm
    {
        push eax
        mov eax, dword ptr[esp + 0x37C + 0x4 + 0x4]
        mov eax, dword ptr[eax + 0x74]
        test eax, eax
        pop eax
        jnz half

        mov dword ptr [esp + 0x37C - 0x1B4], 1280
        jmp end

    half:
        mov dword ptr[esp + 0x37C - 0x1B4], 640

    end:
        mov dword ptr [esp + 0x37C - 0x1B0], 720
        jmp[initializeTargetSurfaceColorD8LastMidAsmHookReturnAddress]
    }
}

uint32_t initializeTargetSurfaceBloomLastMidAsmHookReturnAddress = 0x655A9A;

void __declspec(naked) initializeTargetSurfaceBloomLastMidAsmHook()
{
    __asm
    {
        push eax
        mov eax, dword ptr[esp + 0x37C + 0x4 + 0x4]
        mov eax, dword ptr[eax + 0x74]
        test eax, eax
        pop eax
        jnz half

        mov dword ptr[esp + 0x37C - 0xD4], 1280
        jmp end

    half:
        mov dword ptr[esp + 0x37C - 0xD4], 640

    end:
        mov dword ptr[esp + 0x37C - 0xD0], 720
        jmp[initializeTargetSurfaceBloomLastMidAsmHookReturnAddress]
    }
}

bool BloomScaleFixer::enabled = false;

void BloomScaleFixer::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    ResolutionScaler::applyPatches();

    // FxPipeline
    INSTALL_HOOK(InitializeBloomGlareFrameBuffers);

    // MTFx
    WRITE_JUMP(0x6557D5, initializeTargetSurfaceColorD4LastMidAsmHook);
    WRITE_JUMP(0x65580E, initializeTargetSurfaceColorD8LastMidAsmHook);
    WRITE_JUMP(0x655A8C, initializeTargetSurfaceBloomLastMidAsmHook);
}
