#include "ParameterManager.h"

#include "Configuration.h"

enum class ParameterType
{
    Float,
    Bool
};

struct Parameter
{
    ParameterType type;
    uintptr_t address;
    void* callback;
    union
    {
        float floatValue;
        bool boolValue;
    };
};

typedef void ParameterFloatCallback(float& value);
typedef void ParameterBoolCallback(bool& value);

// Due to the nature of gaussian blur, it'll get weaker
// as resolution increases. We can still make it look
// somewhat the same by scaling the depth of field ranges
// using the resolution factor.
static float s_resolutionFactor = 1.0f;

static void scaleDepthOfFieldParam(float& value)
{
    value *= s_resolutionFactor;
}

// Scene effect overrides.
static void setFinalLightAdjustmentValue(bool& value)
{
    if (Configuration::forceIgnoreFinalLightColorAdjustment)
        value = true;
}

static void setAmbientShadowBiasObject(float& value)
{
    if (Configuration::ambientShadowBiasObject >= 0.0f)
        value = Configuration::ambientShadowBiasObject;

    else if (Configuration::shadowType == ShadowType::Directional)
        value = 0.0f;

    // else leave it be as vertical doesn't always warrant ambient bias.
}

static void setAmbientShadowBiasTerrain(float& value)
{
    if (Configuration::ambientShadowBiasTerrain >= 0.0f)
        value = Configuration::ambientShadowBiasTerrain;

    else if (Configuration::shadowType == ShadowType::Directional)
        value = 0.0f;

    // else leave it be as vertical doesn't always warrant ambient bias.
}

static void setVerticalShadowEnable(bool& value)
{
    if (Configuration::shadowType != ShadowType::Default)
        value = Configuration::shadowType == ShadowType::Vertical;

    ParameterManager::isEnableVerticalShadow = value;
}

static void setLambertShadowObject(bool& value)
{
    if (Configuration::lambertShadowObject != LambertShadowMode::Default)
        value = Configuration::lambertShadowObject == LambertShadowMode::Enable;

    else if (Configuration::shadowType != ShadowType::Default)
        value = Configuration::shadowType == ShadowType::Vertical;

    // else leave it be.
}

static void setLambertShadowTerrain(bool& value)
{
    if (Configuration::lambertShadowTerrain != LambertShadowMode::Default)
        value = Configuration::lambertShadowTerrain == LambertShadowMode::Enable;

    else if (Configuration::shadowType != ShadowType::Default)
        value = Configuration::shadowType == ShadowType::Vertical;

    // else leave it be.
}

static Parameter s_parameters[] =
{
    { ParameterType::Float, 0x1A572E0, scaleDepthOfFieldParam },
    { ParameterType::Float, 0x1A572E4, scaleDepthOfFieldParam },
    { ParameterType::Float, 0x1A572E8, scaleDepthOfFieldParam },
    { ParameterType::Float, 0x1A572EC, scaleDepthOfFieldParam },
    { ParameterType::Float, 0x1A572F0, scaleDepthOfFieldParam },
    { ParameterType::Float, 0x1A572F4, scaleDepthOfFieldParam },
    { ParameterType::Float, 0x1A572F8, scaleDepthOfFieldParam },
    { ParameterType::Float, 0x1A572FC, scaleDepthOfFieldParam },
    { ParameterType::Float, 0x1A43594, scaleDepthOfFieldParam },
    { ParameterType::Float, 0x1A5730C, scaleDepthOfFieldParam },
    { ParameterType::Float, 0x1A57304, scaleDepthOfFieldParam },
    { ParameterType::Float, 0x1A57308, scaleDepthOfFieldParam },
    { ParameterType::Float, 0x1E5E320, scaleDepthOfFieldParam },
    { ParameterType::Bool, 0x1E5E3EA, setFinalLightAdjustmentValue },
    { ParameterType::Float, 0x1E5E32C, setAmbientShadowBiasObject },
    { ParameterType::Float, 0x1E5E328, setAmbientShadowBiasTerrain },
    { ParameterType::Bool, 0x1A43588, setVerticalShadowEnable },
    { ParameterType::Bool, 0x1A4358A, setLambertShadowObject },
    { ParameterType::Bool, 0x1A43589, setLambertShadowTerrain }
};

HOOK(void, __stdcall, LoadSceneEffect, 0x579050, void* A1, void* A2)
{
    ParameterManager::preUpdate();
    originalLoadSceneEffect(A1, A2);
    ParameterManager::postUpdate();
}

void ParameterManager::init()
{
    INSTALL_HOOK(LoadSceneEffect);

    // Fix title screen not rendering at native aspect ratio.
    *reinterpret_cast<float*>(0x18037A0) = 1.0f;
}

void ParameterManager::preUpdate()
{
    // Set callback parameters.
    s_resolutionFactor = 720.0f / static_cast<float>(
        Configuration::height > 0 ? Configuration::height : *reinterpret_cast<int32_t*>(0x1DFDDE0));

    // Backup parameters and run callbacks.
    for (auto& parameter : s_parameters)
    {
        switch (parameter.type)
        {
        case ParameterType::Float:
        {
            float& value = *reinterpret_cast<float*>(parameter.address);
            parameter.floatValue = value;
            reinterpret_cast<ParameterFloatCallback*>(parameter.callback)(value);

            break;
        }

        case ParameterType::Bool:
        {
            bool& value = *reinterpret_cast<bool*>(parameter.address);
            parameter.boolValue = value;
            reinterpret_cast<ParameterBoolCallback*>(parameter.callback)(value);

            break;
        }

        }
    }
}

void ParameterManager::postUpdate()
{
    // Restore parameters.
    for (const auto& parameter : s_parameters)
    {
        switch (parameter.type)
        {
        case ParameterType::Float:
            *reinterpret_cast<float*>(parameter.address) = parameter.floatValue;
            break;

        case ParameterType::Bool:
            *reinterpret_cast<bool*>(parameter.address) = parameter.boolValue;
            break;
        }
    }
}
