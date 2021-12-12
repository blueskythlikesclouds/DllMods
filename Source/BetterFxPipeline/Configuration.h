#pragma once

enum class LambertShadowMode : uint32_t
{
    ENABLE = 0,
    FORCE_DISABLE = 1,
    FORCE_ENABLE = 2
};

enum class FxaaIntensity : uint32_t
{
    DISABLED = 0,
    INTENSITY_0 = 1,
    INTENSITY_1 = 2,
    INTENSITY_2 = 3,
    INTENSITY_3 = 4,
    INTENSITY_4 = 5,
    INTENSITY_5 = 6,
    INTENSITY_6 = 7,
};

enum class BloomType : uint32_t
{
    MTFx = 0,
    SWA = 1,
    FxPipeline = 2
};

enum class ShadowType : uint32_t
{
    Default = 0,
    Directional = 1,
    Vertical = 2
};

class Configuration
{
public:
    static bool forceIgnoreFinalLightColorAdjustment;
    static FxaaIntensity fxaaIntensity;
    static bool postProcessingOnParticles;
    static BloomType bloomType;
    static bool enhancedMotionBlur;

    static uint32_t shadowResolution;
    static float ambientShadowBiasObject;
    static float ambientShadowBiasTerrain;
    static bool enableTerrainShadowCast;
    static bool forceCastShadow;
    static ShadowType shadowType;
    static LambertShadowMode lambertShadowObject;
    static LambertShadowMode lambertShadowTerrain;

    static bool enableResolutionScale;
    static int width;
    static int height;

    static bool load(const std::string& filePath);
};
