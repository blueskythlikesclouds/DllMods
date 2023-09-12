#include "LuminanceRangeFix.h"

// Several materials are missing mrgLuminanceRange
// despite the shader requiring it. This causes flickering.

// FxPipeline handles this edge case by always setting
// the parameter to zero, but MTFx has no such implementation.

// Before IsMadeOne is set to true, we can validate
// the material and add the parameter as necessary.

static std::unordered_set<std::string_view> s_involvedShaderNames =
{
    "Blend_dpdpe1",
    "Blend_dpdpe2",
    "Blend_dpdpe",
    "Blend_dsnbdse1",
    "ChaosV_dsnne1",
    "Chaos_dsbnne1",
    "Chaos_dsnne1",
    "ChrEnamel_dspe1",
    "ChrEnamel_dspe2",
    "ChrEnamel_dspe",
    "ChrEnamel_dspne1",
    "ChrEnamel_dspne",
    "ChrEye_dpne1",
    "ChrEye_dpne2",
    "ChrEye_dpne",
    "ChrMetal_dpe1",
    "ChrMetal_dpe2",
    "ChrMetal_dpe",
    "ChrMetal_dpne2",
    "ChrMetal_dpne",
    "ChrSkin_dse1",
    "ChrSkin_dse2",
    "ChrSkin_dse",
    "ChrSkin_dsfe1",
    "ChrSkin_dsfe2",
    "ChrSkin_dsfe",
    "ChrSkin_dsne1",
    "ChrSkin_dsne",
    "ChrSkin_dsnfe1",
    "ChrSkin_dsnfe",
    "Common_de1",
    "Common_de2",
    "Common_de",
    "Common_dne1",
    "Common_dne2",
    "Common_dne",
    "Common_dpe1",
    "Common_dpe2",
    "Common_dpe",
    "Common_dpne1",
    "Common_dpne2",
    "Common_dpne",
    "Common_dpnne2",
    "Common_dsae1",
    "Common_dse1",
    "Common_dse2",
    "Common_dse",
    "Common_dsne1",
    "Common_dsne",
    "Common_dsnne1",
    "Common_dspe",
    "Common_dspne2",
    "Common_dspne",
    "DimIgnore_e1",
    "Distortion_dsne1",
    "Distortion_dsnne1",
    "EnmCloud_nfe1",
    "EnmEmission_dsle1",
    "EnmEmission_dsnle1",
    "EnmGlass_dse1",
    "EnmGlass_dsle1",
    "EnmGlass_dsne1",
    "EnmGlass_dsnle1",
    "EnmGlass_e1",
    "EnmIgnore_dle1",
    "EnmIgnore_dsle1",
    "EnmIgnore_le1",
    "EnmMetal_dse1",
    "EnmMetal_dse",
    "EnmMetal_dsfe1",
    "EnmMetal_dsfe",
    "EnmMetal_dsne1",
    "EnmMetal_dsne",
    "EnmMetal_dsnfe1",
    "EnmMetal_dsnfe",
    "FakeGlass_dse1",
    "FakeGlass_dsle1",
    "FakeGlass_dsne1",
    "FakeGlass_dsnle1",
    "FakeGlass_e1",
    "GlassRefraction_de1",
    "GlassRefraction_de2",
    "Glass_de2",
    "Glass_de",
    "Glass_dpe2",
    "Glass_dpe",
    "Glass_dpne",
    "Glass_dspe2",
    "Glass_dspe",
    "Glass_dspne",
    "Ice_der",
    "Ice_dne2nr",
    "Ice_dnenr",
    "Ice_dner",
    "Ice_dpne2nr",
    "Ice_dpnenr",
    "Ice_dpnr",
    "LuminescenceV_dpe1",
    "LuminescenceV_dpe2",
    "Luminescence_dne1E",
    "Luminescence_dne2E",
    "Luminescence_dpe1E",
    "Luminescence_dpe2E",
    "Luminescence_dpne1E",
    "Luminescence_dpne2E",
    "MeshParticleRef_d",
    "PamChrEye_dpne1",
    "PamChrEye_dpne2",
    "PamChrEye_dpne",
    "PamChrMetal_dpe1",
    "PamChrMetal_dpe2",
    "PamChrMetal_dpe",
    "PamChrMetal_dpne2",
    "PamChrMetal_dpne",
    "PamChrSkin_dse1",
    "PamChrSkin_dse2",
    "PamChrSkin_dse",
    "PamChrSkin_dsfe1",
    "PamChrSkin_dsfe2",
    "PamChrSkin_dsfe",
    "PamChrSkin_dsne1",
    "PamChrSkin_dsne",
    "PamChrSkin_dsnfe1",
    "PamChrSkin_dsnfe",
    "Ring_dse",
    "TimeEater_dsbnne1"
};

static void __fastcall materialDataSetMadeOne(Hedgehog::Mirage::CMaterialData* materialData)
{
    if (materialData->m_spShaderListData != nullptr)
    {
        const auto& shaderName = materialData->m_spShaderListData->m_TypeAndName;
        const size_t sepIndex = shaderName.find(' ');

        const auto shaderNameView = std::string_view(
            shaderName.data() + sepIndex + 1, shaderName.size() - sepIndex - 1);

        if (s_involvedShaderNames.find(shaderNameView) != s_involvedShaderNames.end())
        {
            const Hedgehog::Base::CStringSymbol luminanceRange("mrgLuminanceRange");
            bool hasParameter = false;

            for (const auto& parameter : materialData->m_Float4Params)
            {
                if (parameter->m_Name.m_pSymbolNode == luminanceRange.m_pSymbolNode)
                {
                    hasParameter = true;
                    break;
                }
            }

            if (!hasParameter)
            {
                const auto float4Param = boost::make_shared<Hedgehog::Mirage::CParameterFloat4Element>();

                float4Param->m_Name = luminanceRange;
                float4Param->m_ValueNum = 1;
                float4Param->m_spValue = boost::make_shared<float[]>(4, 0.0f);

                materialData->m_Float4Params.push_back(float4Param);
            }
        }
    }

    materialData->SetMadeOne();
}

void LuminanceRangeFix::init()
{
    WRITE_JUMP(0x741E00, materialDataSetMadeOne);
    WRITE_JUMP(0x7424DD, materialDataSetMadeOne);
    WRITE_JUMP(0x742BED, materialDataSetMadeOne);
    WRITE_JUMP(0x74170C, materialDataSetMadeOne);
}
