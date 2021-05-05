#pragma once

#include <BlueBlur.h>
#include <Sonic/FxPipeline/Jobs/FxJob.h>

namespace Hedgehog::Yggdrasill
{
    class CYggPicture;
}

namespace Sonic
{
    class CFxToneMapping;

    static FUNCTION_PTR(void, __thiscall, fpCFxToneMappingInitialize, 0x10C5420, CFxToneMapping* This);
    static FUNCTION_PTR(void, __thiscall, fpCFxToneMappingExecute, 0x10C5BB0, CFxToneMapping* This);

    class CFxToneMapping : public CFxJob
    {
    public:
        INSERT_PADDING(0x8);
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggTexture> m_spLuAvgTex;
        INSERT_PADDING(0xF8);
        boost::shared_ptr<Hedgehog::Yggdrasill::CYggPicture> m_spDummyWhiteTex;
        INSERT_PADDING(0xBC);
    };

    ASSERT_OFFSETOF(CFxToneMapping, m_spLuAvgTex, 0x50);
    ASSERT_OFFSETOF(CFxToneMapping, m_spDummyWhiteTex, 0x150);
    ASSERT_SIZEOF(CFxToneMapping, 0x214);
}