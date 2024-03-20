#include "GammaCorrectionImpl.h"
#include "HalfPixelCorrectionImpl.h"
#include "AntiAliasingImpl.h"
#include "CubeMapFix.h"

#include "Configuration.h"
#include "ModLoader.h"

extern "C" void __declspec(dllexport) __cdecl Init(ModInfo_t* modInfo)
{
	if (!Configuration::load())
		MessageBox(nullptr, TEXT("Unable to locate WiiUAccurateVisuals.ini"), TEXT("Wii U Accurate Visuals"), MB_ICONERROR);

	// NOTE: It's important that anti-aliasing gets applied before gamma correction, as we
	// directly modify draw instance params, and gamma correction patch creates a copy of them.

	AntiAliasingImpl::init();
	GammaCorrectionImpl::init();
	HalfPixelCorrectionImpl::init();
	CubeMapFix::init(modInfo);
}