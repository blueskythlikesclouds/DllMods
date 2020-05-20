#include "Configuration.h"
#include "FxaaRenderer.h"
#include "IndependentArchiveLoader.h"

const std::array<const char*, 7> FXAA_SHADER_NAMES =
{
    "FxFXAALite",
    "FxFXAA_0",
    "FxFXAA_1",
    "FxFXAA_2",
    "FxFXAA_3",
    "FxFXAA_4",
    "FxFXAA_5",
};

ShaderDataPair fxaaShaderPair;
boost::shared_ptr<YggTexture> fxaaFrameBuffer;

HOOK(void*, __fastcall, InitializeCrossFade, 0x10C21A0, YggJob* This)
{
    loadShaderDataPair(This->scheduler, fxaaShaderPair, "FxFilterNone", FXAA_SHADER_NAMES[(uint32_t)Configuration::fxaaIntensity - 1]);
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

    IndependentArchiveLoader::applyPatches();

    INSTALL_HOOK(InitializeCrossFade);
    INSTALL_HOOK(ExecuteCrossFade);
}
