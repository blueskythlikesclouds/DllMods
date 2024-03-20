#include "AntiAliasing.h"

#include "Configuration.h"

void AntiAliasing::init()
{
	if (Configuration::fxaaIntensity < FxaaIntensity::Intensity0)
		return;

	// Set FXAA intensity DWORD
	*(uint32_t*)ASLR(0x11D7B24) = (uint32_t)Configuration::fxaaIntensity - (uint32_t)FxaaIntensity::Intensity0;

	// Use linear filtering
	WRITE_MEMORY(ASLR(0xEB52DD), uint8_t, (uint8_t)(*(uint8_t*)ASLR(0xEB52DD) | 0x80));
	WRITE_MEMORY(ASLR(0xEB52DF), uint8_t, (uint8_t)(*(uint8_t*)ASLR(0xEB52DF) | 0x80));

	// Remove conditionals to always render AA
	WRITE_MEMORY(ASLR(0xEBEAC8), uint32_t, 0);
	WRITE_MEMORY(ASLR(0xEBEACC), uint32_t, 0);
}