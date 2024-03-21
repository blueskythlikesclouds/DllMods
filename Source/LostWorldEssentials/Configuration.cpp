#include "Configuration.h"

bool Configuration::load()
{
    const INIReader reader("LostWorldEssentials.ini");

    if (reader.ParseError() != 0)
        return false;

    gammaCorrection = reader.GetBoolean("WiiUAccurateVisuals", "GammaCorrection", true);
    halfPixelCorrection = reader.GetBoolean("WiiUAccurateVisuals", "HalfPixelCorrection", true);
    correctCubemaps = reader.GetBoolean("WiiUAccurateVisuals", "CorrectCubemaps", true);
    depthOfField = reader.GetBoolean("WiiUAccurateVisuals", "DepthOfField", true);

    fxaaIntensity = static_cast<FxaaIntensity>(reader.GetInteger("GraphicsTweaks", "FxaaIntensity", static_cast<uint32_t>(FxaaIntensity::Disabled)));
    profilePictureQuality = static_cast<PfpQuality>(reader.GetInteger("GraphicsTweaks", "ProfilePictureQuality", static_cast<uint32_t>(PfpQuality::High)));

    return true;
}
