#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Yggdrasill/hhYggScheduler.h>
#include <Sonic/FxPipeline/Jobs/RenderScene/FxRenderScene.h>

namespace Hedgehog::Yggdrasill
{
    class CYggSurface;
    class CYggTexture;
}

namespace Sonic
{
    class CFxRenderGameScene;

    static FUNCTION_PTR(void, __thiscall, fpCFxRenderGameSceneInitialize, 0x10C8A00, CFxRenderGameScene* This);
    static FUNCTION_PTR(void, __thiscall, fpCFxRenderGameSceneExecute, 0x10CA5A0, CFxRenderGameScene* This);

    class CFxRenderGameScene : public CFxRenderScene
    {
    public:
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggTexture> m_spColorTex;
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggTexture> m_spDepthTex;
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggTexture> m_spCapturedColorTex;
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggTexture> m_spCapturedDepthTex;
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggTexture> m_spBlendTex;
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggSurface> m_spColorSurface;
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggSurface> m_spDepthSurface;
        Hedgehog::Mirage::SShaderPair m_BlendColorShader;
        INSERT_PADDING(0x7C);
    };

    ASSERT_OFFSETOF(CFxRenderGameScene, m_spColorTex, 0x70);
    ASSERT_OFFSETOF(CFxRenderGameScene, m_spDepthTex, 0x78);
    ASSERT_OFFSETOF(CFxRenderGameScene, m_spCapturedColorTex, 0x80);
    ASSERT_OFFSETOF(CFxRenderGameScene, m_spCapturedDepthTex, 0x88);
    ASSERT_OFFSETOF(CFxRenderGameScene, m_spBlendTex, 0x90);
    ASSERT_OFFSETOF(CFxRenderGameScene, m_spColorSurface, 0x98);
    ASSERT_OFFSETOF(CFxRenderGameScene, m_spDepthSurface, 0xA0);
    ASSERT_OFFSETOF(CFxRenderGameScene, m_BlendColorShader, 0xA8);
    ASSERT_SIZEOF(CFxRenderGameScene, 0x134);
}