#include "ProfilePictureQuality.h"

#include "Configuration.h"

static int RetAddress = (int)ASLR(0x52537C);

__declspec(naked) void ProfilePictureQualityHook()
{
    __asm
    {
        mov esi, [ebp - 0x10]
        imul esi, [ebp - 0x1C]
        mov ebx, ecx
        shl ebx, 2
        add esi, ebx
        add esi, [ebp - 0x18]

        mov bl, [eax + 3]
        movzx ebx, bl

        jmp[RetAddress]
    }
}

void ProfilePictureQuality::init()
{
    switch (Configuration::profilePictureQuality)
    {
    case PfpQuality::Low:
    {
        // Change SteamAPI call offset to GetSmallFriendAvatar function call.
        WRITE_MEMORY(ASLR(0x52528E), uint8_t, 0x88);

        break;
    }
    case PfpQuality::Medium:
    {
        // Change SteamAPI call offset to GetMediumFriendAvatar function call.
        // (This is the game's default but we restore it in-case other codes change it.)
        WRITE_MEMORY(ASLR(0x52528E), uint8_t, 0x8C);

        break;
    }
    case PfpQuality::High:
    {
        // Change SteamAPI call offset to GetLargeFriendAvatar function call.
        WRITE_MEMORY(ASLR(0x52528E), uint8_t, 0x90);

        WRITE_JUMP(ASLR(0x525376), &ProfilePictureQualityHook);

        break;
    }
    }
}