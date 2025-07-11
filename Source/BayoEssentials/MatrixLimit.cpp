#include "MatrixLimit.h"

static void __declspec(naked) setSkinningMtxMidAsmHook()
{
    static uint32_t returnAddr = 0x4A00A7;
    __asm
    {
        mov eax, dword ptr [esi + 58h]
        mov ecx, dword ptr [eax]
        cmp ecx, 0FFFFFFFFh
        jne moveByte
        lea ecx, [eax + edi * 2]
        movzx ecx, word ptr [ecx + 4]
        jmp returnBack
    moveByte:
        movzx ecx, byte ptr[eax + edi]
    returnBack:
        jmp[returnAddr]
    }
}

static void __declspec(naked) maxMtxMidAsmHook()
{
    static uint32_t returnAddr = 0x4A17F6;
    __asm
    {
        mov ecx, dword ptr [edx]
        cmp ecx, 0FFFFFFFFh
        jne moveByte
        lea ecx, [edx + eax * 2]
        movzx ecx, word ptr [ecx + 4]
        jmp returnBack
    moveByte:
        movzx ecx, byte ptr [edx + eax]
    returnBack:
        cmp ebx, ecx
        jmp[returnAddr]
    }
}

void MatrixLimit::init()
{
    WRITE_JUMP(0x4A00A0, setSkinningMtxMidAsmHook);
    WRITE_JUMP(0x4A17F0, maxMtxMidAsmHook);
}