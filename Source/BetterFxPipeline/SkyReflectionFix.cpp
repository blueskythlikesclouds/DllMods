#include "SkyReflectionFix.h"

// Sky models in reflections get clipped
// for some reason and it looks horrible.

// We can fix it up by overriding the matrix manually.

struct ProjViewInfo
{
    Hedgehog::Math::CMatrix44 projection[3];
    Hedgehog::Math::CMatrix44 skyProjection[3];
    Hedgehog::Math::CMatrix44 view;
    Hedgehog::Math::CVector eyeDirection;
    Hedgehog::Math::CVector eyePosition;
    BB_INSERT_PADDING(0x10);
};
BB_ASSERT_SIZEOF(ProjViewInfo, 0x1F0);

HOOK(void, __fastcall, GenerateProjViewInfo, 0x658D10, void* This, void* _, ProjViewInfo* projViewInfos, void* A3, void* A4, void* A5)
{
    originalGenerateProjViewInfo(This, _, projViewInfos, A3, A4, A5);

    for (size_t i = 0; i < 2; i++)
    {
        ProjViewInfo& projViewInfo = projViewInfos[4 + i];

        for (size_t j = 0; j < 3; j++)
        {
            // Prevent clipping.
            projViewInfo.skyProjection[j](1, 0) = 0.0f;
            projViewInfo.skyProjection[j](2, 0) = 0.0f;
            projViewInfo.skyProjection[j](3, 0) = 0.0f;

            projViewInfo.skyProjection[j](0, 1) = 0.0f;
            projViewInfo.skyProjection[j](2, 1) = 0.0f;
            projViewInfo.skyProjection[j](3, 1) = 0.0f;

            projViewInfo.skyProjection[j](0, 2) = 0.0f;
            projViewInfo.skyProjection[j](1, 2) = 0.0f;

            projViewInfo.skyProjection[j](0, 3) = 0.0f;
            projViewInfo.skyProjection[j](1, 3) = 0.0f;
            projViewInfo.skyProjection[j](3, 3) = 0.0f;
        }
    }
}

void SkyReflectionFix::init()
{
    INSTALL_HOOK(GenerateProjViewInfo);
}
