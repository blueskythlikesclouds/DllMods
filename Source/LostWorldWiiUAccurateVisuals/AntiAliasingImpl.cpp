#include "AntiAliasingImpl.h"

#include "Configuration.h"

struct SDrawInstanceParam
{
	uint32_t reserved;

	const char* name;
	uint32_t id;

	void* callback;

	void* childParams;
	uint32_t childParamCount;

	uint8_t rt0;
	uint8_t rt1;

	uint8_t rt2;
	uint8_t rt3;

	uint8_t s0;
	uint8_t s1;
	uint8_t s2;
	uint8_t s3;

	uint32_t unk1;
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
	uint32_t unk5;
	uint32_t unk6;
};

void injectDrawInstanceParam(SDrawInstanceParam* drawInstanceParam)
{
	static_cast<SDrawInstanceParam*>(drawInstanceParam->childParams)[26].unk5 = 0;
	static_cast<SDrawInstanceParam*>(drawInstanceParam->childParams)[26].unk6 = 0;
}

void AntiAliasingImpl::init()
{
	if (Configuration::fxaaIntensity < FxaaIntensity::Intensity0)
		return;

	if (Configuration::fxaaLinearFiltering)
	{
		WRITE_MEMORY(ASLR(0xEB52DC), uint8_t, *(uint8_t*)ASLR(0xEB52DC) | 0x80);
		WRITE_MEMORY(ASLR(0xEB52DE), uint8_t, *(uint8_t*)ASLR(0xEB52DE) | 0x80);
	}

	injectDrawInstanceParam((SDrawInstanceParam*)ASLR(0xEBF158));
}