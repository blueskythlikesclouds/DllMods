#include "QuickBoot.h"

static uint32_t s_r000MidAsmHookRetAddr = 0x5A1D3A;
static bool s_handledR000MidAsmHook = false;

static void __declspec(naked) r000MidAsmHook()
{
    __asm
    {
        xor edi, edi
        mov[esi + 0x14], edi
        cmp s_handledR000MidAsmHook, 1
        jnz cond
        jmp[s_r000MidAsmHookRetAddr]
    cond:
        mov s_handledR000MidAsmHook, 1

        mov[esi + 0x24], 2
        mov[esi + 0x10], 4
        mov[esi + 0xC], edi
        mov[esi + 0x8], 4
        pop edi
        pop ebx
        pop esi
        retn
    }
}

void QuickBoot::init(const INIReader& reader)
{
    bool skipLogo = true;
    bool autoSaveQuickBoot = false;

    if (reader.ParseError() == 0)
    {
        skipLogo = reader.GetBoolean("Main", "SkipLogo", skipLogo);
        autoSaveQuickBoot = reader.GetBoolean("Main", "AutoSaveQuickBoot", autoSaveQuickBoot);
    }

    if (skipLogo || autoSaveQuickBoot)
    {
        // logo + movie skip
        WRITE_MEMORY(0x459CE5, uint8_t, 0x83, 0xC4, 0x10, 0x90, 0x90);
        WRITE_MEMORY(0x459D07, uint8_t, 0xEB);
        WRITE_MEMORY(0x5A1C40, uint8_t, 0xC);
        WRITE_MEMORY(0x5A1C43, uint8_t, 0x8, 0x1);

        if (autoSaveQuickBoot)
            WRITE_JUMP(0x5A1D35, r000MidAsmHook);
    }
}