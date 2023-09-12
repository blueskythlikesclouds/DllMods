#pragma once

enum class LambertShadowMode : uint32_t
{
    Default = 0,
    Disable = 1,
    Enable = 2
};

enum class FxaaIntensity : uint32_t
{
    Disabled = 0,
    Intensity0 = 1,
    Intensity1 = 2,
    Intensity2 = 3,
    Intensity3 = 4,
    Intensity4 = 5,
    Intensity5 = 6,
    Intensity6 = 7,
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
    static inline bool forceIgnoreFinalLightColorAdjustment;
    static inline FxaaIntensity fxaaIntensity;
    static inline bool postProcessingOnParticles;
    static inline BloomType bloomType;
    static inline bool enhancedMotionBlur;

    static inline uint32_t shadowResolution = 4096;
    static inline float ambientShadowBiasObject = -1.0f;
    static inline float ambientShadowBiasTerrain = -1.0f;
    static inline bool enableTerrainShadowCast;
    static inline bool forceCastShadow;
    static inline ShadowType shadowType;
    static inline LambertShadowMode lambertShadowObject;
    static inline LambertShadowMode lambertShadowTerrain;

    static inline bool enableResolutionScale;
    static inline int32_t width = -1;
    static inline int32_t height = -1;

    static bool load();
};
