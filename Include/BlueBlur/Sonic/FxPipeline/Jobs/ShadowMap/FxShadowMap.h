#pragma once

#include <BlueBlur.h>
#include <Sonic/FxPipeline/Jobs/FxJob.h>

namespace Hedgehog::Yggdrasill
{
    class CYggSurface;
    class CYggTexture;
}

namespace Sonic
{
    class CFxShadowMap;

    static FUNCTION_PTR(void, __thiscall, fpCFxShadowMapInitialize, 0x10C6CE0, CFxShadowMap* This);
    static FUNCTION_PTR(void, __thiscall, fpCFxShadowMapExecute, 0x10C68D0, CFxShadowMap* This);

    class CFxShadowMap : public CFxJob
    {
    public:
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggSurface> m_spColorTex;
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggTexture> m_spShadowMap;
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggTexture> m_spShadowMapNoTerrain;
        INSERT_PADDING(0x44);
        uint32_t m_ShadowMapSize;
        INSERT_PADDING(0x4);
    };

    ASSERT_OFFSETOF(CFxShadowMap, m_spColorTex, 0x48);
    ASSERT_OFFSETOF(CFxShadowMap, m_spShadowMap, 0x50);
    ASSERT_OFFSETOF(CFxShadowMap, m_spShadowMapNoTerrain, 0x58);
    ASSERT_OFFSETOF(CFxShadowMap, m_ShadowMapSize, 0xA4);
    ASSERT_SIZEOF(CFxShadowMap, 0xAC);
}