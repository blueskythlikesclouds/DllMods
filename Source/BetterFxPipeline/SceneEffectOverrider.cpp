#include "Configuration.h"
#include "SceneEffectOverrider.h"

void SceneEffectOverrider::update()
{
    if (Configuration::forceIgnoreFinalLightColorAdjustment)
        *(bool*)0x1E5E3EA = true;

    if (Configuration::shadowType == ShadowType::Directional || Configuration::ambientShadowBiasObject >= 0)
        *(float*)0x1E5E32C = Configuration::ambientShadowBiasObject >= 0 ? Configuration::ambientShadowBiasObject : 0.0f;

    if (Configuration::shadowType == ShadowType::Directional || Configuration::ambientShadowBiasTerrain >= 0)
        *(float*)0x1E5E328 = Configuration::ambientShadowBiasTerrain >= 0 ? Configuration::ambientShadowBiasTerrain : 0.0f;

    if (Configuration::shadowType != ShadowType::Default)
        *(bool*)0x1A43588 = Configuration::shadowType == ShadowType::Vertical;

    if (Configuration::shadowType != ShadowType::Default || Configuration::lambertShadowObject != LambertShadowMode::ENABLE)
        *(bool*)0x1A4358A = (Configuration::lambertShadowObject == LambertShadowMode::ENABLE && Configuration::shadowType == ShadowType::Vertical) || 
            Configuration::lambertShadowObject == LambertShadowMode::FORCE_ENABLE;

    if (Configuration::shadowType != ShadowType::Default || Configuration::lambertShadowTerrain != LambertShadowMode::ENABLE)
        *(bool*)0x1A43589 = (Configuration::lambertShadowTerrain == LambertShadowMode::ENABLE && Configuration::shadowType == ShadowType::Vertical) ||
        Configuration::lambertShadowTerrain == LambertShadowMode::FORCE_ENABLE;
}
