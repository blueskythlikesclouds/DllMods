#include "Configuration.h"

bool Configuration::load()
{
    const INIReader reader("BetterFxPipeline.ini");

    if (reader.ParseError() != 0)
        return false;

    forceIgnoreFinalLightColorAdjustment = reader.GetBoolean("Renderer", "ForceIgnoreFinalLightColorAdjustment", false);
    fxaaIntensity = static_cast<FxaaIntensity>(reader.GetInteger("Renderer", "FxaaIntensity", static_cast<uint32_t>(FxaaIntensity::Disabled)));
    postProcessingOnParticles = reader.GetBoolean("Renderer", "PostProcessingOnParticles", false);
    bloomType = static_cast<BloomType>(reader.GetInteger("Renderer", "BloomType", 0));
    enhancedMotionBlur = reader.GetBoolean("Renderer", "EnhancedMotionBlur", false);

    shadowResolution = static_cast<uint32_t>(std::max(1l, reader.GetInteger("Shadows", "ShadowResolution", 4096)));
    ambientShadowBiasObject = reader.GetFloat("Shadows", "AmbientShadowBiasObject", -1.0f);
    ambientShadowBiasTerrain = reader.GetFloat("Shadows", "AmbientShadowBiasTerrain", -1.0f);
    enableTerrainShadowCast = reader.GetBoolean("Shadows", "EnableTerrainShadowCast", false);
    forceCastShadow = reader.GetBoolean("Shadows", "ForceCastShadow", false);
    shadowType = static_cast<ShadowType>(reader.GetInteger("Shadows", "ShadowType", static_cast<long>(ShadowType::Default)));
    lambertShadowObject = static_cast<LambertShadowMode>(reader.GetInteger("Shadows", "LambertShadowObject", static_cast<uint32_t>(LambertShadowMode::Default)));
    lambertShadowTerrain = static_cast<LambertShadowMode>(reader.GetInteger("Shadows", "LambertShadowTerrain", static_cast<uint32_t>(LambertShadowMode::Default)));

    enableResolutionScale = reader.GetBoolean("InternalResolution", "Scale", false);
    width = enableResolutionScale ? reader.GetInteger("InternalResolution", "Width", -1) : -1;
    height = enableResolutionScale ? reader.GetInteger("InternalResolution", "height", -1) : -1;

    return true;
}
