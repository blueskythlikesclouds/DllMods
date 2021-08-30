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

    // Ignore Devil's Details' FXAA shader
    WRITE_NOP(0x64CC19, 2);

    if (Configuration::fxaaIntensity <= FxaaIntensity::DISABLED || 
        Configuration::fxaaIntensity > FxaaIntensity::INTENSITY_6)
        return;

    // FxPipeline
    INSTALL_HOOK(InitializeCrossFade);
    INSTALL_HOOK(ExecuteCrossFade);

    // MTFx
    {
        hh::fx::SScreenRenderParam* newScreenRenderParam = new hh::fx::SScreenRenderParam();
        memcpy(newScreenRenderParam, (void*)0x13DF5A8, sizeof(hh::fx::SScreenRenderParam));

        // Refer to ShaderLoader.cpp for shader indices
        newScreenRenderParam->m_ShaderIndex = 0x350;

        // Insert our own Draw Instance Param to Render Before Particle 3
        hh::fx::SDrawInstanceParam* renderBeforeParticle3Param = (hh::fx::SDrawInstanceParam*)0x13DDDC8;

        hh::fx::SDrawInstanceParam* newChildren = new hh::fx::SDrawInstanceParam[renderBeforeParticle3Param->m_ChildParamCount + 2];
        memcpy(newChildren, renderBeforeParticle3Param->m_ChildParams, sizeof(hh::fx::SDrawInstanceParam) * renderBeforeParticle3Param->m_ChildParamCount);

        // Initialize FXAA parameters
        hh::fx::SDrawInstanceParam* fxaaParam = &newChildren[renderBeforeParticle3Param->m_ChildParamCount];
        memset(fxaaParam, 0, sizeof(hh::fx::SDrawInstanceParam));

        fxaaParam->m_pCallback = (void*)0x651820;
        fxaaParam->m_ChildParams = newScreenRenderParam;
        fxaaParam->m_RenderTargetSurface = 7;
        fxaaParam->m_MsaaRenderTargetSurface = 7;
        fxaaParam->m_S0Sampler = 0x80 | 10;
        fxaaParam->m_Unk0 = 0x3;
        fxaaParam->m_Unk2 = 0x101;

        hh::fx::SDrawInstanceParam* copyParam = &newChildren[renderBeforeParticle3Param->m_ChildParamCount + 1];
        memset(copyParam, 0, sizeof(hh::fx::SDrawInstanceParam));

        copyParam->m_pCallback = (void*)0x651820;
        copyParam->m_ChildParams = (void*)0x13DF5A8;
        copyParam->m_RenderTargetSurface = 10;
        copyParam->m_MsaaRenderTargetSurface = 10;
        copyParam->m_S0Sampler = 7;
        copyParam->m_Unk0 = 0x3;
        copyParam->m_Unk2 = 0x101;

        // Pass new pointers
        WRITE_MEMORY(&renderBeforeParticle3Param->m_ChildParams, void*, newChildren);
        WRITE_MEMORY(&renderBeforeParticle3Param->m_ChildParamCount, uint32_t, renderBeforeParticle3Param->m_ChildParamCount + 2);
    }
}
