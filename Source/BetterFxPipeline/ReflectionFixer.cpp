#include "ReflectionFixer.h"

struct ProjViewInfo
{
    Eigen::Matrix4f projMatrices[3];
    Eigen::Matrix4f skyProjMatrices[3];
    Eigen::Matrix4f view;
    Eigen::Vector4f eyeDirection;
    Eigen::Vector4f eyePosition;
    INSERT_PADDING(0x10);
};
ASSERT_SIZEOF(ProjViewInfo, 0x1F0);

HOOK(void, __fastcall, GenerateProjViewInfo, 0x658D10, void* This, void* Edx, ProjViewInfo* projViewInfos, void* A3, void* A4, void* A5)
{    
    originalGenerateProjViewInfo(This, Edx, projViewInfos, A3, A4, A5);

    for (size_t i = 0; i < 2; i++)
    {
        ProjViewInfo& refProjViewInfo = projViewInfos[4 + i];

        for (size_t j = 0; j < 3; j++)
        {
            // Prevent clipping
            refProjViewInfo.skyProjMatrices[j](1, 0) = 0.0f;
            refProjViewInfo.skyProjMatrices[j](2, 0) = 0.0f;
            refProjViewInfo.skyProjMatrices[j](3, 0) = 0.0f;

            refProjViewInfo.skyProjMatrices[j](0, 1) = 0.0f;
            refProjViewInfo.skyProjMatrices[j](2, 1) = 0.0f;
            refProjViewInfo.skyProjMatrices[j](3, 1) = 0.0f;

            refProjViewInfo.skyProjMatrices[j](0, 2) = 0.0f;
            refProjViewInfo.skyProjMatrices[j](1, 2) = 0.0f;

            refProjViewInfo.skyProjMatrices[j](0, 3) = 0.0f;
            refProjViewInfo.skyProjMatrices[j](1, 3) = 0.0f;
            refProjViewInfo.skyProjMatrices[j](3, 3) = 0.0f;
        }
    }
}

bool ReflectionFixer::enabled = false;

void ReflectionFixer::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    INSTALL_HOOK(GenerateProjViewInfo);
}
