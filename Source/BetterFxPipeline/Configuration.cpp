#include "Configuration.h"

bool Configuration::fixBloomScale = true;
bool Configuration::fixDofScale = true;
bool Configuration::forceIgnoreFinalLightColorAdjustment = false;
bool Configuration::postProcessingOnParticles = true;
BloomType Configuration::bloomType = BloomType::MTFx;

FxaaIntensity Configuration::fxaaIntensity = FxaaIntensity::DISABLED;

float Configuration::ambientShadowBiasObject = -1;
float Configuration::ambientShadowBiasTerrain = -1;
bool Configuration::enableTerrainShadowCast = false;
bool Configuration::forceCastShadow = false;
bool Configuration::forceDirectionalShadow = true;
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

    fixBloomScale = reader.GetBoolean("Renderer", "FixBloomScale", true);
    fixDofScale = reader.GetBoolean("Renderer", "FixDOFScale", true);
    forceIgnoreFinalLightColorAdjustment = reader.GetBoolean("Renderer", "ForceIgnoreFinalLightColorAdjustment", false);
    fxaaIntensity = (FxaaIntensity)reader.GetInteger("Renderer", "FxaaIntensity", (uint32_t)FxaaIntensity::DISABLED);
    postProcessingOnParticles = reader.GetBoolean("Renderer", "PostProcessingOnParticles", true);
    bloomType = (BloomType)reader.GetInteger("Renderer", "BloomType", 0);

    ambientShadowBiasObject = reader.GetFloat("Shadows", "AmbientShadowBiasObject", -1);
    ambientShadowBiasTerrain = reader.GetFloat("Shadows", "AmbientShadowBiasTerrain", -1);
    enableTerrainShadowCast = reader.GetBoolean("Shadows", "EnableTerrainShadowCast", false);
    forceCastShadow = reader.GetBoolean("Shadows", "ForceCastShadow", false);
    forceDirectionalShadow = reader.GetBoolean("Shadows", "ForceDirectionalShadow", true);
    lambertShadowObject = (LambertShadowMode)reader.GetInteger("Shadows", "LambertShadowObject", (uint32_t)LambertShadowMode::ENABLE);
    lambertShadowTerrain = (LambertShadowMode)reader.GetInteger("Shadows", "LambertShadowTerrain", (uint32_t)LambertShadowMode::ENABLE);

    enableResolutionScale = reader.GetBoolean("InternalResolution", "Scale", false);
    width = enableResolutionScale ? reader.GetInteger("InternalResolution", "Width", -1) : -1;
    height = enableResolutionScale ? reader.GetInteger("InternalResolution", "height", -1) : -1;

    return true;
}
