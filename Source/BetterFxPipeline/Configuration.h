#pragma once

enum LambertShadowMode : uint32_t
{
    ENABLE = 0,
    FORCE_DISABLE = 1,
    FORCE_ENABLE = 2
};

class Configuration
{
public:
    static bool fixBloomScale;
    static bool fixDofScale;
    static bool forceIgnoreFinalLightColorAdjustment;
    static bool postProcessingOnParticles;

    static float ambientShadowBiasObject;
    static float ambientShadowBiasTerrain;
    static bool enableTerrainShadowCast;
    static bool forceCastShadow;
    static bool forceDirectionalShadow;
    static LambertShadowMode lambertShadowObject;
    static LambertShadowMode lambertShadowTerrain;

    static bool enableResolutionScale;
    static int width;
    static int height;

    static bool load(const std::string& filePath);
};
