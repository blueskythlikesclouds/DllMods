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
	Intensity6 = 7
};

enum class PfpQuality : uint32_t
{
	Low = 0,
	Medium = 1,
	High = 2
};

class Configuration
{
public:
	static inline bool gammaCorrection = true;
	static inline bool halfPixelCorrection = true;
	static inline bool correctCubemaps = true;
	static inline bool depthOfField = true;

	static inline FxaaIntensity fxaaIntensity = FxaaIntensity::Disabled;
	static inline PfpQuality profilePictureQuality = PfpQuality::High;

	static bool load();
};