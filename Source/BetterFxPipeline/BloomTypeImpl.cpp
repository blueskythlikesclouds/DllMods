#include "BloomTypeImpl.h"

#include "Configuration.h"

HOOK(void, __cdecl, CreateBloomStarParam, 0x110E670, Sonic::CParameterGroup* This)
{
    originalCreateBloomStarParam(This);

    Sonic::CEditParam* editParam = This->CreateParameterCategory("Extra", "");

    editParam->CreateParamTypeList(&BloomTypeImpl::s_bloomType, "BloomType", "",
        {
            { "Better FxPipeline", 0 },
            { "Sonic Generations", static_cast<uint32_t>(BloomType::MTFx) + 1 },
            { "Sonic Unleashed", static_cast<uint32_t>(BloomType::SWA) + 1 },
            { "FxPipeline Renderer (Legacy)", static_cast<uint32_t>(BloomType::FxPipeline) + 1 },
        });

    This->Flush();
}

static FUNCTION_PTR(void, __cdecl, screenDefaultExec, 0x6517E0, void*, void*);

static void __cdecl makeBrightPassHDRScreenDefaultExec(void* A1, void* A2)
{
    Hedgehog::Mirage::CMirageDatabaseWrapper wrapper(
        Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spShaderDatabase.get());

    const auto shader = wrapper.GetPixelShaderData("HfBloom_BrightPassHDR");

    auto bloomType = Configuration::bloomType;

    if (BloomTypeImpl::s_bloomType > 0 && BloomTypeImpl::s_bloomType <= static_cast<uint32_t>(BloomType::FxPipeline) + 1)
        bloomType = static_cast<BloomType>(BloomTypeImpl::s_bloomType - 1);

    switch (bloomType)
    {
    case BloomType::MTFx:
    default:
        shader->m_spCode = wrapper.GetPixelShaderCodeData("HfBloom_BrightPassHDR");
        break;

    case BloomType::SWA:
        shader->m_spCode = wrapper.GetPixelShaderCodeData("SWA_Bloom_BrightPassHDR");
        break;

    case BloomType::FxPipeline:
        shader->m_spCode = wrapper.GetPixelShaderCodeData("Bloom_BrightPassHDR");
        break;
    }

    screenDefaultExec(A1, A2);
}

void BloomTypeImpl::init()
{
    INSTALL_HOOK(CreateBloomStarParam);
    WRITE_MEMORY(0x1803C4C, void*, makeBrightPassHDRScreenDefaultExec);
}
