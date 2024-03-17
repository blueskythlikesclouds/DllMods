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
	static inline FxaaIntensity fxaaIntensity;
	static inline bool fxaaLinearFiltering{};
	static inline bool gammaCorrection{};
	static inline bool halfPixelCorrection{};
	static inline bool correctCubemaps{};

	static bool load();
};