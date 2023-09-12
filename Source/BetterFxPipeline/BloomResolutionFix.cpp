#include "BloomResolutionFix.h"

// To have bloom blur correctly, we need to fix the resolution to 720p.
// However, it goes bonkers if the framebuffers are larger than the internal resolution,
// so we need to check whether we should use that resolution first.

// TODO: 3D mode might be making the checks incorrect... need to ensure it works

static uint32_t s_initializeTargetSurfaceColorD4LastMidAsmHookReturnAddress = 0x6557E3;

static void __declspec(naked) initializeTargetSurfaceColorD4LastMidAsmHook()
{
    __asm
    {
        cmp edi, 1280
        jl moveOriginalRes
        cmp ebp, 720
        jl moveOriginalRes

        // 3D mode
        push eax
        mov eax, dword ptr[esp + 0x37C + 0x4 + 0x4]
        mov eax, dword ptr[eax + 0x74]
        test eax, eax
        pop eax
        jnz moveHalfWidth

        mov dword ptr [esp + 0x37C - 0x1D0], 1280
        jmp moveHeight

    moveHalfWidth:
        mov dword ptr[esp + 0x37C - 0x1D0], 640

    moveHeight:
        mov dword ptr [esp + 0x37C - 0x1CC], 720
        jmp end

    moveOriginalRes:
        mov dword ptr[esp + 0x37C - 0x1D0], edi
        mov dword ptr[esp + 0x37C - 0x1CC], ebp
    end:
        jmp[s_initializeTargetSurfaceColorD4LastMidAsmHookReturnAddress]
    }
}

static uint32_t s_initializeTargetSurfaceColorD8LastMidAsmHookReturnAddress = 0x65581C;

static void __declspec(naked) initializeTargetSurfaceColorD8LastMidAsmHook()
{
    __asm
    {
        cmp edi, 1280
        jl moveOriginalRes
        cmp ebp, 720
        jl moveOriginalRes

        // 3D mode
        push eax
        mov eax, dword ptr[esp + 0x37C + 0x4 + 0x4]
        mov eax, dword ptr[eax + 0x74]
        test eax, eax
        pop eax
        jnz moveHalfWidth

        mov dword ptr [esp + 0x37C - 0x1B4], 1280
        jmp moveHeight

    moveHalfWidth:
        mov dword ptr[esp + 0x37C - 0x1B4], 640

    moveHeight:
        mov dword ptr [esp + 0x37C - 0x1B0], 720
        jmp end

    moveOriginalRes:
        mov dword ptr[esp + 0x37C - 0x1B4], edi
        mov dword ptr[esp + 0x37C - 0x1B0], ebp
    end:
        jmp[s_initializeTargetSurfaceColorD8LastMidAsmHookReturnAddress]
    }
}

static uint32_t s_initializeTargetSurfaceBloomLastMidAsmHookReturnAddress = 0x655A9A;

static void __declspec(naked) initializeTargetSurfaceBloomLastMidAsmHook()
{
    __asm
    {
        cmp edi, 1280
        jl moveOriginalRes
        cmp ebp, 720
        jl moveOriginalRes

        // 3D mode
        push eax
        mov eax, dword ptr[esp + 0x37C + 0x4 + 0x4]
        mov eax, dword ptr[eax + 0x74]
        test eax, eax
        pop eax
        jnz moveHalfWidth

        mov dword ptr [esp + 0x37C - 0xD4], 1280
        jmp moveHeight

    moveHalfWidth:
        mov dword ptr[esp + 0x37C - 0xD4], 640

    moveHeight:
        mov dword ptr [esp + 0x37C - 0xD0], 720
        jmp end

    moveOriginalRes:
        mov dword ptr[esp + 0x37C - 0xD4], edi
        mov dword ptr[esp + 0x37C - 0xD0], ebp
    end:
        jmp[s_initializeTargetSurfaceBloomLastMidAsmHookReturnAddress]
    }
}


void BloomResolutionFix::init()
{
    WRITE_JUMP(0x6557D5, initializeTargetSurfaceColorD4LastMidAsmHook);
    WRITE_JUMP(0x65580E, initializeTargetSurfaceColorD8LastMidAsmHook);
    WRITE_JUMP(0x655A8C, initializeTargetSurfaceBloomLastMidAsmHook);
}
