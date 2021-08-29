#include "Configuration.h"
#include "FxaaRenderer.h"

const std::array<const char*, 7> FxaaRenderer::SHADER_NAMES =
{
    "FxFXAALite",
    "FxFXAA_0",
    "FxFXAA_1",
    "FxFXAA_2",
    "FxFXAA_3",
    "FxFXAA_4",
    "FxFXAA_5",
};

//
// FxPipeline
// 

ShaderDataPair fxaaShaderPair;
boost::shared_ptr<YggTexture> fxaaFrameBuffer;

HOOK(void*, __fastcall, InitializeCrossFade, 0x10C21A0, YggJob* This)
{
    loadShaderDataPair(This->scheduler, fxaaShaderPair, "FxFilterNone", FxaaRenderer::SHADER_NAMES[(uint32_t)Configuration::fxaaIntensity - 1]);
    createYggTexture(This->scheduler->internal->subInternal, fxaaFrameBuffer, 1, 1, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, NULL);

    return originalInitializeCrossFade(This);
}

HOOK(void*, __fastcall, ExecuteCrossFade, 0x10C22D0, YggJob* This)
{
    void* result = originalExecuteCrossFade(This);

    if (!fxaaShaderPair.first || !fxaaShaderPair.second || !fxaaFrameBuffer)
        return result;

    boost::shared_ptr<YggTexture> frameBuffer;
    getDefaultFramebuffer(This, frameBuffer);

    boost::shared_ptr<YggSurface> surface;
    getYggSurface(fxaaFrameBuffer.get(), surface, 0, 0);

    setRenderTarget(This->scheduler->internal->subInternal, 0, surface);
    setShader(This->scheduler->internal->subInternal, fxaaShaderPair.first, fxaaShaderPair.second);

    setTexture(This->scheduler->internal->subInternal, 0, frameBuffer);
    setSamplerFilter(This->scheduler->internal->subInternal, 0, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_NONE);
    setSamplerAddressMode(This->scheduler->internal->subInternal, 0, D3DTADDRESS_CLAMP);

    renderQuad(This->scheduler->internal->subInternal, 0, 0, 0);

    setDefaultFramebuffer(This, fxaaFrameBuffer);

    return result;
}

bool FxaaRenderer::enabled = false;

void FxaaRenderer::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    // Ignore Devil's Details' FXAA implementation
    WRITE_NOP(0x64CC19, 2);

    if (Configuration::fxaaIntensity <= FxaaIntensity::DISABLED || 
        Configuration::fxaaIntensity > FxaaIntensity::INTENSITY_6)
        return;

    // FxPipeline
    INSTALL_HOOK(InitializeCrossFade);
    INSTALL_HOOK(ExecuteCrossFade);

    // MTFx
    {
        hh::fx::SScreenRenderParam* newScreenRenderParam = (hh::fx::SScreenRenderParam*)operator new(
            sizeof(hh::fx::SScreenRenderParam));
        memcpy(newScreenRenderParam, (void*)0x13DF5A8, sizeof(hh::fx::SScreenRenderParam));

        // Refer to ShaderLoader.cpp for shader indices
        newScreenRenderParam->m_ShaderIndex = 0x350;

        // Insert our own Draw Instance Param to Color Correction
        hh::fx::SDrawInstanceParam* colorCorrectionParam = (hh::fx::SDrawInstanceParam*)0x13E06A8;
        hh::fx::SDrawInstanceParam* children = (hh::fx::SDrawInstanceParam*)colorCorrectionParam->m_ChildParams;

        hh::fx::SDrawInstanceParam* newChildren = new hh::fx::SDrawInstanceParam[colorCorrectionParam->
            m_ChildParamCount + 1];

        // Copy original children & set their render targets to TARGETSURFACE_COLOR1
        for (size_t i = 0; i < colorCorrectionParam->m_ChildParamCount; i++)
        {
            newChildren[i] = children[i];
            newChildren[i].m_RenderTargetSurface = 12;
            newChildren[i].m_MsaaRenderTargetSurface = 12;
        }

        // Initialize FXAA parameters
        hh::fx::SDrawInstanceParam* newChild =
            &newChildren[colorCorrectionParam->m_ChildParamCount];

        *newChild = children[0];
        newChild->m_RenderTargetSurface = 3;
        newChild->m_MsaaRenderTargetSurface = 3;
        newChild->m_S0Sampler = 12;
        newChild->m_S1Sampler = 0;
        newChild->m_S2Sampler = 0;
        newChild->m_S3Sampler = 0;
        newChild->m_ChildParams = newScreenRenderParam;
        newChild->m_Unk4 = 0;
        newChild->m_Unk5 = 0;

        // Pass new pointers
        WRITE_MEMORY(&colorCorrectionParam->m_ChildParams, void*, newChildren);
        WRITE_MEMORY(&colorCorrectionParam->m_ChildParamCount, uint32_t, colorCorrectionParam->m_ChildParamCount + 1);
    }
}
