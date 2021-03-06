#include "BloomScaleFixer.h"
#include "Configuration.h"
#include "DirectionalShadowFixer.h"
#include "DofScaleFixer.h"
#include "FxaaRenderer.h"
#include "FxPipelineEnabler.h"
#include "LoadingScreenFixer.h"
#include "LostCodeLoader.h"
#include "ParameterFixer.h"
#include "ParticlePostProcessor.h"
#include "ResolutionScaler.h"
#include "SceneEffectOverrider.h"
#include "ShadowCaster.h"
#include "BloomTypeHandler.h"
#include "TransparentShadowFixer.h"

extern "C" __declspec(dllexport) void __cdecl OnFrame()
{
    SceneEffectOverrider::update();
}

extern "C" __declspec(dllexport) void __cdecl Init(ModInfo *info)
{
    std::string dir = info->CurrentMod->Path;

    size_t pos = dir.find_last_of("\\/");
    if (pos != std::string::npos)
        dir.erase(pos + 1);

    if (!Configuration::load(dir + "BetterFxPipeline.ini"))
        MessageBox(NULL, L"Failed to parse BetterFxPipeline.ini", NULL, MB_ICONERROR);

    FxPipelineEnabler::applyPatches();

    DirectionalShadowFixer::applyPatches();

    ParameterFixer::applyPatches();

    TransparentShadowFixer::applyPatches();

    if (Configuration::fixBloomScale)
        BloomScaleFixer::applyPatches();

    if (Configuration::fixDofScale)
        DofScaleFixer::applyPatches();

    if (Configuration::postProcessingOnParticles)
        ParticlePostProcessor::applyPatches();

    BloomTypeHandler::applyPatches();

    if (Configuration::fxaaIntensity > FxaaIntensity::DISABLED &&
        Configuration::fxaaIntensity <= FxaaIntensity::INTENSITY_6)
        FxaaRenderer::applyPatches();

    ShadowCaster::applyPatches();

    if (Configuration::enableResolutionScale)
        ResolutionScaler::applyPatches();

    LoadingScreenFixer::applyPatches();
}