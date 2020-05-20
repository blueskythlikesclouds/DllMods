#include "Configuration.h"
#include "SceneEffectOverrider.h"

void SceneEffectOverrider::update()
{
    if (Configuration::forceIgnoreFinalLightColorAdjustment)
        *(bool*)0x1E5E3EA = true;

    if (Configuration::ambientShadowBiasObject >= 0)
        *(float*)0x1E5E32C = Configuration::ambientShadowBiasObject;

    if (Configuration::ambientShadowBiasTerrain >= 0)
        *(float*)0x1E5E328 = Configuration::ambientShadowBiasTerrain;

    if (Configuration::forceDirectionalShadow)
        *(bool*)0x1A43588 = false;

    if (Configuration::lambertShadowObject != LambertShadowMode::ENABLE)
        *(bool*)0x1A4358A = Configuration::lambertShadowObject == LambertShadowMode::FORCE_ENABLE;

    if (Configuration::lambertShadowTerrain != LambertShadowMode::ENABLE)
        *(bool*)0x1A43589 = Configuration::lambertShadowTerrain == LambertShadowMode::FORCE_ENABLE;
}
