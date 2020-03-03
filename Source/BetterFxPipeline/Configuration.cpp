#include "Configuration.h"

bool Configuration::fixBloomScale = true;
bool Configuration::fixDofScale = true;
bool Configuration::forceIgnoreFinalLightColorAdjustment = false;
bool Configuration::postProcessingOnParticles = true;

float Configuration::ambientShadowBiasObject = -1;
float Configuration::ambientShadowBiasTerrain = -1;
bool Configuration::enableTerrainShadowCast = false;
bool Configuration::forceCastShadow = false;
bool Configuration::forceDirectionalShadow = true;
LambertShadowMode Configuration::lambertShadowObject = ENABLE;
LambertShadowMode Configuration::lambertShadowTerrain = ENABLE;

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
    postProcessingOnParticles = reader.GetBoolean("Renderer", "PostProcessingOnParticles", true);

    ambientShadowBiasObject = reader.GetFloat("Shadows", "AmbientShadowBiasObject", -1);
    ambientShadowBiasTerrain = reader.GetFloat("Shadows", "AmbientShadowBiasTerrain", -1);
    enableTerrainShadowCast = reader.GetBoolean("Shadows", "EnableTerrainShadowCast", false);
    forceCastShadow = reader.GetBoolean("Shadows", "ForceCastShadow", false);
    forceDirectionalShadow = reader.GetBoolean("Shadows", "ForceDirectionalShadow", true);
    lambertShadowObject = static_cast<LambertShadowMode>(reader.GetInteger("Shadows", "LambertShadowObject", ENABLE));
    lambertShadowTerrain = static_cast<LambertShadowMode>(reader.GetInteger("Shadows", "LambertShadowTerrain", ENABLE));

    enableResolutionScale = reader.GetBoolean("InternalResolution", "Scale", false);
    width = enableResolutionScale ? reader.GetInteger("InternalResolution", "Width", -1) : -1;
    height = enableResolutionScale ? reader.GetInteger("InternalResolution", "height", -1) : -1;

    return true;
}
