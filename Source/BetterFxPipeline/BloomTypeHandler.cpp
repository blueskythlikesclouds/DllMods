#include "BloomTypeHandler.h"
#include "Configuration.h"

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

hh::mr::SShaderPair bbBloom;
hh::mr::SShaderPair swaBloom;
hh::mr::SShaderPair legacyBloom;

void setBloomShaderPair(hh::mr::SShaderPair& shaderPair)
{
    const BloomType bloomType = sceneEffectBloomType > 0 && sceneEffectBloomType <= (uint32_t)BloomType::FxPipeline + 1 ?
        (BloomType)(sceneEffectBloomType - 1) : Configuration::bloomType;

    switch (bloomType)
    {
    case BloomType::MTFx:
        shaderPair = bbBloom;
        break;

    case BloomType::SWA:
        shaderPair = swaBloom;
        break;

    default:
        shaderPair = legacyBloom;
        break;
    }
}

//
// FxPipeline
//

HOOK(void, __fastcall, CFxBloomGlareInitialize, Sonic::fpCFxBloomGlareInitialize, Sonic::CFxBloomGlare* This)
{
    originalCFxBloomGlareInitialize(This);

    This->m_pScheduler->GetShader(bbBloom, "RenderBuffer", "HfBloom_BrightPassHDR");
    This->m_pScheduler->GetShader(swaBloom, "RenderBuffer", "SWA_Bloom_BrightPassHDR");
    This->m_pScheduler->GetShader(legacyBloom, "RenderBuffer", "Bloom_BrightPassHDR");
}

HOOK(void, __fastcall, CFxBloomGlareExecute, Sonic::fpCFxBloomGlareExecute, Sonic::CFxBloomGlare* This)
{
    setBloomShaderPair(This->m_Bloom_BrightPassHDRShader);
    originalCFxBloomGlareExecute(This);
}

//
// MTFx
//

hh::mr::SShaderPair* bloomShaderPairRef;

namespace
{
    HOOK(void, __fastcall, MTFxInitializeRenderBufferShaders, 0x651320, void* This, void* Edx, hh::db::CDatabase* pDatabase)
    {
        hh::mr::CMirageDatabaseWrapper wrapper(pDatabase);

        bbBloom.m_spVertexShader = wrapper.GetVertexShaderData("RenderBuffer");
        bbBloom.m_spPixelShader = wrapper.GetPixelShaderData("HfBloom_BrightPassHDR");

        swaBloom.m_spVertexShader = wrapper.GetVertexShaderData("RenderBuffer");
        swaBloom.m_spPixelShader = wrapper.GetPixelShaderData("SWA_Bloom_BrightPassHDR");

        legacyBloom.m_spVertexShader = wrapper.GetVertexShaderData("RenderBuffer");
        legacyBloom.m_spPixelShader = wrapper.GetPixelShaderData("Bloom_BrightPassHDR");

        bloomShaderPairRef = (hh::mr::SShaderPair*)((char*)This + 27 * 16);

        originalMTFxInitializeRenderBufferShaders(This, Edx, pDatabase);
    }
}

bool BloomTypeHandler::enabled = false;

void BloomTypeHandler::update()
{
    if (!enabled || !bloomShaderPairRef) return;

    setBloomShaderPair(*bloomShaderPairRef);
}

void BloomTypeHandler::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    INSTALL_HOOK(CreateBloomStarParams);

    INSTALL_HOOK(CFxBloomGlareInitialize);
    INSTALL_HOOK(CFxBloomGlareExecute);

    INSTALL_HOOK(MTFxInitializeRenderBufferShaders);
}
