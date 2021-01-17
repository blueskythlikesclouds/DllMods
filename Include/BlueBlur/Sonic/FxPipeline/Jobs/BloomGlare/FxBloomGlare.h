#pragma once

#include <BlueBlur.h>
#include <Sonic/FxPipeline/Jobs/FxJob.h>

namespace Sonic
{
    class CFxBloomGlare;

    static FUNCTION_PTR(void, __thiscall, fpCFxBloomGlareInitialize, 0x10D4130, CFxBloomGlare* This);
    static FUNCTION_PTR(void, __thiscall, fpCFxBloomGlareExecute, 0x10D3C20, CFxBloomGlare* This);

    class CFxBloomGlare : public CFxJob
    {
    public:
        INSERT_PADDING(0x98);
        Hedgehog::Mirage::SShaderPair m_Bloom_BrightPassHDRShader;
        INSERT_PADDING(0x300);
    };

    ASSERT_OFFSETOF(CFxBloomGlare, m_Bloom_BrightPassHDRShader, 0xE0);
    ASSERT_SIZEOF(CFxBloomGlare, 0x3F0);
}