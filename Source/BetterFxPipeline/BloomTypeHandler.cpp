#include "BloomTypeHandler.h"
#include "Configuration.h"
#include "IndependentArchiveLoader.h"

uint32_t sceneEffectBloomType;

HOOK(void, __cdecl, CreateBloomStarParams, 0x110E670, Sonic::CParameterGroup* This)
{
    originalCreateBloomStarParams(This);

    Sonic::CParameterCategory* pParameterCategory = This->CreateParameterCategory("Extra", "");

    pParameterCategory->CreateParamTypeList(&sceneEffectBloomType, "BloomType", "",
    {
        { "Better FxPipeline", 0 },
        { "Sonic Generations", (uint32_t)BloomType::MTFx + 1 },
        { "Sonic Unleashed", (uint32_t)BloomType::SWA + 1 },
        { "FxPipeline Renderer (Legacy)", (uint32_t)BloomType::FxPipeline + 1 },
    });

    This->Flush();
}

Hedgehog::Mirage::SShaderPair s_HfBloom_BrightPassHDRShader;
Hedgehog::Mirage::SShaderPair s_SWA_Bloom_BrightPassHDRShader;
Hedgehog::Mirage::SShaderPair s_Bloom_BrightPassHDRShader;

HOOK(void, __fastcall, CFxBloomGlareInitialize, Sonic::fpCFxBloomGlareInitialize, Sonic::CFxBloomGlare* This)
{
    originalCFxBloomGlareInitialize(This);

    This->m_pScheduler->GetShader(s_HfBloom_BrightPassHDRShader, "RenderBuffer", "HfBloom_BrightPassHDR");
    This->m_pScheduler->GetShader(s_SWA_Bloom_BrightPassHDRShader, "RenderBuffer", "SWA_Bloom_BrightPassHDR");
    This->m_pScheduler->GetShader(s_Bloom_BrightPassHDRShader, "RenderBuffer", "Bloom_BrightPassHDR");
}

HOOK(void, __fastcall, CFxBloomGlareExecute, Sonic::fpCFxBloomGlareExecute, Sonic::CFxBloomGlare* This)
{
    const BloomType bloomType = sceneEffectBloomType > 0 && sceneEffectBloomType <= (uint32_t)BloomType::FxPipeline + 1 ? 
        (BloomType)(sceneEffectBloomType - 1) : Configuration::bloomType;

    switch (bloomType)
    {
    case BloomType::MTFx:
        This->m_Bloom_BrightPassHDRShader = s_HfBloom_BrightPassHDRShader;
        break;

    case BloomType::SWA:
        This->m_Bloom_BrightPassHDRShader = s_SWA_Bloom_BrightPassHDRShader;
        break;

    default:
        This->m_Bloom_BrightPassHDRShader = s_Bloom_BrightPassHDRShader;
        break;
    }

    originalCFxBloomGlareExecute(This);
}

bool BloomTypeHandler::enabled = false;

void BloomTypeHandler::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    IndependentArchiveLoader::applyPatches();

    INSTALL_HOOK(CreateBloomStarParams);

    INSTALL_HOOK(CFxBloomGlareInitialize);
    INSTALL_HOOK(CFxBloomGlareExecute);
}
