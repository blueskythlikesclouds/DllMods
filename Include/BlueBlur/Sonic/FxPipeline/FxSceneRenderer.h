#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Yggdrasill/hhYggSceneRenderer.h>

namespace Sonic
{
    class CFxSceneRenderer : public Hedgehog::Yggdrasill::CYggSceneRenderer
    {
    public:
        INSERT_PADDING(0xC);
        uint8_t m_BackgroundColor[4];
        INSERT_PADDING(0x40);
        Eigen::Matrix4f m_LightViewProjection;
        Eigen::Matrix4f m_VerticalLightViewProjection;
        INSERT_PADDING(0x90);
    };

    ASSERT_OFFSETOF(CFxSceneRenderer, m_BackgroundColor, 0xBC);
    ASSERT_OFFSETOF(CFxSceneRenderer, m_LightViewProjection, 0x100);
    ASSERT_OFFSETOF(CFxSceneRenderer, m_VerticalLightViewProjection, 0x140);
    ASSERT_SIZEOF(CFxSceneRenderer, 0x210);
}