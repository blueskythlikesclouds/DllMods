#include "Configuration.h"

bool Configuration::forceIgnoreFinalLightColorAdjustment = false;
bool Configuration::postProcessingOnParticles = false;
BloomType Configuration::bloomType = BloomType::MTFx;

FxaaIntensity Configuration::fxaaIntensity = FxaaIntensity::DISABLED;

uint32_t Configuration::shadowResolution = 4096;
float Configuration::ambientShadowBiasObject = -1;
float Configuration::ambientShadowBiasTerrain = -1;
bool Configuration::enableTerrainShadowCast = false;
bool Configuration::forceCastShadow = false;
ShadowType Configuration::shadowType = ShadowType::Default;
LambertShadowMode Configuration::lambertShadowObject = LambertShadowMode::ENABLE;
LambertShadowMode Configuration::lambertShadowTerrain = LambertShadowMode::ENABLE;

bool Configuration::enableResolutionScale = false;
int Configuration::width = -1;
int Configuration::height = -1;

bool Configuration::load(const std::string& filePath)
{
    const INIReader reader(filePath);
    if (reader.ParseError() != 0)
        return false;

    forceIgnoreFinalLightColorAdjustment = reader.GetBoolean("Renderer", "ForceIgnoreFinalLightColorAdjustment", false);
    fxaaIntensity = (FxaaIntensity)reader.GetInteger("Renderer", "FxaaIntensity", (uint32_t)FxaaIntensity::DISABLED);
    postProcessingOnParticles = reader.GetBoolean("Renderer", "PostProcessingOnParticles", false);
    bloomType = (BloomType)reader.GetInteger("Renderer", "BloomType", 0);

    shadowResolution = (uint32_t)max(1, reader.GetInteger("Shadows", "ShadowResolution", 4096));
    ambientShadowBiasObject = reader.GetFloat("Shadows", "AmbientShadowBiasObject", -1);
    ambientShadowBiasTerrain = reader.GetFloat("Shadows", "AmbientShadowBiasTerrain", -1);
    enableTerrainShadowCast = reader.GetBoolean("Shadows", "EnableTerrainShadowCast", false);
    forceCastShadow = reader.GetBoolean("Shadows", "ForceCastShadow", false);
    shadowType = (ShadowType)reader.GetInteger("Shadows", "ShadowType", (long)ShadowType::Default);
    lambertShadowObject = (LambertShadowMode)reader.GetInteger("Shadows", "LambertShadowObject", (uint32_t)LambertShadowMode::ENABLE);
    lambertShadowTerrain = (LambertShadowMode)reader.GetInteger("Shadows", "LambertShadowTerrain", (uint32_t)LambertShadowMode::ENABLE);

    enableResolutionScale = reader.GetBoolean("InternalResolution", "Scale", false);
    width = enableResolutionScale ? reader.GetInteger("InternalResolution", "Width", -1) : -1;
    height = enableResolutionScale ? reader.GetInteger("InternalResolution", "height", -1) : -1;

    return true;
}
