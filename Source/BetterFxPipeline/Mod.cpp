#include "BloomScaleFixer.h"
#include "Configuration.h"
#include "DirectionalShadowFixer.h"
#include "DofScaleFixer.h"
#include "ForceAlphaColorFixer.h"
#include "FxPipelineEnabler.h"
#include "LostCodeLoader.h"
#include "ParticlePostProcessor.h"
#include "ResolutionScaler.h"
#include "SceneEffectOverrider.h"
#include "ShadowCaster.h"
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

    // Swap MTFx to FxPipeline
    FxPipelineEnabler::applyPatches();

    // Fix black directional shadows
    DirectionalShadowFixer::applyPatches();

    // Fix g_ForceAlphaColor
    ForceAlphaColorFixer::applyPatches();

    // Patch the transparent shadow shader to ignore mrgTexcoordOffset
    TransparentShadowFixer::applyPatches();

    if (Configuration::fixBloomScale)
        BloomScaleFixer::applyPatches();

    if (Configuration::fixDofScale)
        DofScaleFixer::applyPatches();

    if (Configuration::postProcessingOnParticles)
        ParticlePostProcessor::applyPatches();

    ShadowCaster::applyPatches();

    if (Configuration::enableResolutionScale)
        ResolutionScaler::applyPatches();
}