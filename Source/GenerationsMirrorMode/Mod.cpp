#include "../../Source/GenerationsFreeCamera/Camera.h"

HOOK(void, __fastcall, UpdateCamera, 0x10FB770, Camera* This, void* Edx, uint32_t a2)
{
    originalUpdateCamera(This, Edx, a2);
    This->projectionMatrix(0, 0) = -abs(This->projectionMatrix(0, 0));
}

HOOK(void, __fastcall, SetProjectionMatrix, 0x6FA3E0, hh::mr::CRenderingDevice* This, void* Edx, Eigen::Matrix4f& projection)
{
    if (projection(2, 3) != 0.0f) // Check if it's perspective projection
        projection(0, 0) = -abs(projection(0, 0));

    return originalSetProjectionMatrix(This, Edx, projection);
}

// Copies and reverts the flip
void __fastcall matrixCopyImpl(Eigen::Matrix4f& to, void* Edx, const Eigen::Matrix4f& from)
{
    to = from;
    to(0, 0) = abs(to(0, 0));
}

// Negates aspect ratio to simulate flipping
uint32_t getCameraAspectRatioAddr = 0xD4DB30;

void __declspec(naked) getCameraAspectRatioImpl()
{
    __asm
    {
        call[getCameraAspectRatioAddr]
        xorps xmm0, ds:0x1703930
        retn
    }
}

namespace
{
    void flip(size_t& flags, const size_t left, const size_t right)
    {
        const bool hasLeft = (flags & left) != 0;
        const bool hasRight = (flags & right) != 0;

        flags &= ~(left | right);

        if (hasLeft) flags |= right;
        if (hasRight) flags |= left;
    }

    void flip(size_t& flags)
    {
        flip(flags, app::eKeyState_DpadLeft, app::eKeyState_DpadRight);
        flip(flags, app::eKeyState_LeftBumper, app::eKeyState_RightBumper);
    }
}

HOOK(bool, __fastcall, ManagerImpDI8Update, 0x9C85D0, void* This, void* Edx, char* padStates, uint32_t padStateCount, void* A4)
{
    const bool result = originalManagerImpDI8Update(This, Edx, padStates, padStateCount, A4);

    for (size_t i = 0; i < padStateCount; i++)
    {
        char* padState = padStates + 20 * i;

        flip(*(size_t*)(padState + 4)); // Key Down

        *(int16_t*)(padState + 8) *= -1; // Left X
        *(int16_t*)(padState + 12) *= -1; // Right X
    }

    return result;
}

HOOK(void, __fastcall, MsgStartQuickStepSign, 0x528870, void* This, void* Edx, uint32_t a2)
{
    uint32_t* type = (uint32_t*)(a2 + 48);

    if (*type == 1)
        *type = 0;

    else if (*type == 0)
        *type = 1;

    originalMsgStartQuickStepSign(This, Edx, a2);
}

HOOK(void, __fastcall, MsgStartCommonButtonSign, 0x5289A0, void* This, void* Edx, uint32_t a2)
{
    uint32_t* type = (uint32_t*)(a2 + 24);

    if (*type == 1)
        *type = 2;

    else if (*type == 2)
        *type = 1;

    originalMsgStartCommonButtonSign(This, Edx, a2);
}

extern "C" void __declspec(dllexport) PostInit()
{
    // Mirror main camera
    INSTALL_HOOK(UpdateCamera);

    // Mirror title camera (should be scrapped preferably)
    INSTALL_HOOK(SetProjectionMatrix);

    // Revert mirroring in CVisibilityCullingManager (freaks out otherwise)
    WRITE_CALL(0xD15C7A, matrixCopyImpl);

    // Revert mirroring in CInstanceRenderObjDX9 (freaks out otherwise)
    WRITE_CALL(0xCD936F, matrixCopyImpl);

    // Fix input
    INSTALL_HOOK(ManagerImpDI8Update);

    // Flip backface culling
    WRITE_MEMORY(0x6FEA8B, uint8_t, 2); // CW -> CCW
    WRITE_MEMORY(0x6FEA91, uint8_t, 3); // CCW -> CW

    // Flip boost particles & shadow rival orb etc.
    if (*(uint32_t*)0x11AED1F == 0xB9EE0CE8)
        WRITE_CALL(0x11AED1F, getCameraAspectRatioImpl);

    //
    // Fix keyboard input
    //

    // Bumper
    WRITE_MEMORY(0xA5762D, uint8_t, 0x38);
    WRITE_MEMORY(0xA57654, uint8_t, 0x30);

    // D-pad
    WRITE_MEMORY(0xA5772A, uint8_t, 0x68);
    WRITE_MEMORY(0xA57751, uint8_t, 0x60);

    //
    // Fix options input
    //

    // BGM/Sound slider
    WRITE_MEMORY(0xD1E7F7, uint8_t, 0x75);

    // Brightness slider
    WRITE_MEMORY(0xD20163, uint8_t, 0x75);

    // Menu option left <-> right
    WRITE_MEMORY(0x10BA677, uint32_t, 0x100000);
    WRITE_MEMORY(0x10BA687, uint32_t, 0x200000);

    //
    // Flip navigation icons (authored by brianuuu)
    //
    INSTALL_HOOK(MsgStartQuickStepSign);
    INSTALL_HOOK(MsgStartCommonButtonSign);

    // LB/RB in stage select
    WRITE_MEMORY(0x107C41C, uint8_t, 0x1);
    WRITE_MEMORY(0x107C544, uint8_t, 0x1);
    WRITE_MEMORY(0x107FE0E, uint8_t, 0x1);
}