#pragma once

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

class Configuration
{
public:
	static inline bool gammaCorrection = true;
	static inline bool halfPixelCorrection = true;
	static inline bool correctCubemaps = true;

	static inline FxaaIntensity fxaaIntensity = FxaaIntensity::Disabled;

	static bool load();
};