#pragma once

#include <BlueBlur.h>

namespace Hedgehog::FxRenderFramework
{
    struct SDrawInstanceParam
    {
        uint32_t m_Reserved0; // Temporarily set by game
        uint32_t m_Reserved1; // Temporarily set by game

        const char* m_pName;
        uint32_t m_Id;

        void* m_pCallback; // ScreenTraverse

        void* m_ChildParams;
        uint32_t m_ChildParamCount;

        INSERT_PADDING(0x4);

        uint8_t m_DepthStencilSurface;
        uint8_t m_RenderTargetSurface;

        uint8_t m_MsaaDepthStencilSurface;
        uint8_t m_MsaaRenderTargetSurface;

        union
        {
            struct
            {
                uint8_t m_ReflectionMap2Sampler;
                uint8_t m_ReflectionMapSampler;

                uint8_t m_VerticalShadowMapSampler;
                uint8_t m_ShadowMapSampler;
            };

            struct
            {
                uint8_t m_S3Sampler;
                uint8_t m_S2Sampler;
                uint8_t m_S1Sampler;
                uint8_t m_S0Sampler;
            };
        };

        uint32_t m_Unk0;
        uint32_t m_Unk1;
        uint32_t m_Unk2;
        uint32_t m_Unk3;
        uint32_t m_Unk4;
        uint32_t m_Unk5;
    };

    ASSERT_SIZEOF(SDrawInstanceParam, 0x40);
}