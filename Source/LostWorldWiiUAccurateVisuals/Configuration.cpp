#include "Configuration.h"

bool Configuration::load()
{
    const INIReader reader("LostWorldWiiUAccurateVisuals.ini");

    if (reader.ParseError() != 0)
        return false;

    fxaaIntensity = static_cast<FxaaIntensity>(reader.GetInteger("WiiUAccurateVisuals", "FxaaIntensity", static_cast<uint32_t>(FxaaIntensity::Intensity0)));
    fxaaLinearFiltering = reader.GetBoolean("WiiUAccurateVisuals", "FxaaLinearFiltering", false);
    gammaCorrection = reader.GetBoolean("WiiUAccurateVisuals", "GammaCorrection", true);
    halfPixelCorrection = reader.GetBoolean("WiiUAccurateVisuals", "HalfPixelCorrection", true);
    correctCubemaps = reader.GetBoolean("WiiUAccurateVisuals", "CorrectCubemaps", true);

    return true;
}
