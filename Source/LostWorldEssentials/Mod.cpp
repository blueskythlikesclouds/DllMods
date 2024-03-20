#include "AntiAliasing.h"
#include "Configuration.h"
#include "CubeMapFix.h"
#include "GammaCorrection.h"
#include "HalfPixelCorrection.h"
#include "ModLoader.h"
#include "WindowFix.h"

extern "C" void __declspec(dllexport) __cdecl Init(ModInfo_t* modInfo)
{
	if (!Configuration::load())
		MessageBox(nullptr, TEXT("Unable to locate LostWorldEssentials.ini"), TEXT("LostWorldEssentials"), MB_ICONERROR);

	// The nvapi.dll is loaded so that the game is forced to use the dedicated GPU in the system.
    // This is done to avoid graphical issues presented by older integrated GPUs.
	LoadLibraryA("nvapi.dll");

	// NOTE: It's important that anti-aliasing gets applied before gamma correction, as we
	// directly modify draw instance params, and gamma correction patch creates a copy of them.
	AntiAliasing::init();
	GammaCorrection::init(modInfo);
	HalfPixelCorrection::init(modInfo);
	CubeMapFix::init(modInfo);
	WindowFix::init();
}
