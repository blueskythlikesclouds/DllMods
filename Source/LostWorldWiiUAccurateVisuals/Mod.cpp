#include "GammaCorrectionImpl.h"
#include "HalfPixelCorrectionImpl.h"
#include "AntiAliasingImpl.h"
#include "CorrectCubemapsImpl.h"

#include "Configuration.h"
#include "ModLoader.h"

extern "C"
{
	void __declspec(dllexport) __cdecl Init(ModInfo_t* modInfo)
	{
		if (!Configuration::load())
			MessageBox(nullptr, TEXT("Unable to locate WiiUAccurateVisuals.ini"), TEXT("Wii U Accurate Visuals"), MB_ICONERROR);

		GammaCorrectionImpl::init();
		HalfPixelCorrectionImpl::init();
		AntiAliasingImpl::init();
		CorrectCubemapsImpl::init(modInfo);
	}
}