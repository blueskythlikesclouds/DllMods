#include "DepthOfFieldFix.h"

#include "Configuration.h"

static int DofRes = (int)ASLR(0xFEFC7C);
static int GrassRes = (int)ASLR(0xFB6D1C);
static int ShadowRes = (int)ASLR(0xFB6D1D);
static int RetAddress = (int)ASLR(0x904E94);

__declspec(naked) void FixDepthOfFieldDrawOrderHook()
{
    __asm
    {
        mov al, byte ptr [edi + 28Ch]
        mov edx, [DofRes]
        mov edx, [edx]
        and al, dl
        mov[edi + 28Ch], al
        mov eax, [GrassRes]
        cmp [eax], 0
        sete cl
        mov[edi + 49Ch], cl
        mov eax, [ShadowRes]
        mov eax, [eax]
        mov[edi + 4F4h], al

        jmp [RetAddress]
    }
}

void DepthOfFieldFix::init()
{
	if (!Configuration::depthOfField)
		return;

    WRITE_JUMP(ASLR(0x904E64), &FixDepthOfFieldDrawOrderHook);
}
