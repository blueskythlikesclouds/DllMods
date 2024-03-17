#include "GammaCorrectionImpl.h"

#include "Configuration.h"

struct ShaderInfo
{
	size_t index;
	const char* vertexShaderName;
	const char* pixelShaderName;
};

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

struct ScreenRenderParam
{
	const char* name;
	void* callback;
	size_t shaderIndex;
	INSERT_PADDING(0x18);
};



FUNCTION_PTR(void, __cdecl, SceneTraversed_ScreenDefault, ASLR(0xC48F70), void* sceneRenderInfo, const ScreenRenderParam* screenRenderParam);

void __cdecl SceneTraversed_ScreenGammaCorrection(void* sceneRenderInfo, const ScreenRenderParam* screenRenderParam, bool drc)
{
	float gamma = 1.25f;

	if (const auto sceneParameters = app::xgame::SceneParameters::GetInstance())
	{
		if (const auto sceneData = sceneParameters->GetSceneData())
			gamma = 1.0f / (drc ? sceneData->config.gammaDRCWiiU : sceneData->config.gammaTVWiiU);
	}

	const float gammaConstData[] =
	{
		gamma,
		gamma,
		gamma,
		1.0f
	};

	app::ApplicationWin::GetInstance()->GetDirect3DDevice()->SetPixelShaderConstantF(150, gammaConstData, 1);

	SceneTraversed_ScreenDefault(sceneRenderInfo, screenRenderParam);
}

void __cdecl SceneTraversed_ScreenGammaCorrectionTV(void* sceneRenderInfo, const ScreenRenderParam* screenRenderParam)
{
	SceneTraversed_ScreenGammaCorrection(sceneRenderInfo, screenRenderParam, false);
}

void __cdecl SceneTraversed_ScreenGammaCorrectionDRC(void* sceneRenderInfo, const ScreenRenderParam* screenRenderParam)
{
	SceneTraversed_ScreenGammaCorrection(sceneRenderInfo, screenRenderParam, true);
}

ScreenRenderParam gammaCorrectionScreenRenderParamTV =
{
	"gamma correction tv",
	(void*)&SceneTraversed_ScreenGammaCorrectionTV,
	0, // set in Init
	{}
};

SDrawInstanceParam gammaCorrectionDrawInstanceParamTV =
{
	0,
	"ScreenScreenStateFlags::Copy",
	0,
	(void*)ASLR(0xC58F50),
	&gammaCorrectionScreenRenderParamTV,
	0,

	0,
	0x0B,
	0,
	0x0C,

	0x00,
	0x00,
	0x00,
	0x0B,

	0,
	0x8000000,
	0x101,
	0,
	0,
	0
};

#if 0
ScreenRenderParam gammaCorrectionScreenRenderParamDRC =
{
	"gamma correction drc",
	(void*)&SceneTraversed_ScreenGammaCorrectionDRC,
	0, // set in Init
	{}
};

SDrawInstanceParam gammaCorrectionDrawInstanceParamDRC =
{
	0,
	"ScreenScreenStateFlags::Copy",
	0,
	(void*)ASLR(0xC58F50),
	&gammaCorrectionScreenRenderParamDRC,
	0,

	0,
	0x04,
	0,
	0x04,

	0x00,
	0x00,
	0x00,
	0x04,

	0,
	0x8000000,
	0x101,
	0,
	0,
	0
};
#endif

void injectDrawInstanceParam(SDrawInstanceParam* drawInstanceParam, bool drc)
{
	SDrawInstanceParam* newDrawInstanceParams = (SDrawInstanceParam*)operator new(sizeof(SDrawInstanceParam) * (drawInstanceParam->childParamCount + 1));
	memcpy(newDrawInstanceParams, drawInstanceParam->childParams, sizeof(SDrawInstanceParam) * drawInstanceParam->childParamCount);

#if 0
	memcpy(&newDrawInstanceParams[drawInstanceParam->childParamCount], drc ? &gammaCorrectionDrawInstanceParamDRC : &gammaCorrectionDrawInstanceParamTV, sizeof(SDrawInstanceParam));
#else
	memcpy(&newDrawInstanceParams[drawInstanceParam->childParamCount], &gammaCorrectionDrawInstanceParamTV, sizeof(SDrawInstanceParam));
#endif

	WRITE_MEMORY(&drawInstanceParam->childParams, SDrawInstanceParam*, newDrawInstanceParams);
	WRITE_MEMORY(&drawInstanceParam->childParamCount, size_t, drawInstanceParam->childParamCount + 1);
}

void GammaCorrectionImpl::init()
{
	if (!Configuration::gammaCorrection)
		return;

	// Check if patches were applied already (safety measure for mods that include this DLL)
	if (strcmp(*(char**)ASLR(0xEB7DEC), "FxGammaCorrection") == 0)
		return;

	//
	// Inject FxGammaCorrection
	//
#if 0 // Broken
	size_t* const byteSize = (size_t*)ASLR(0xC506A4);
	WRITE_MEMORY(byteSize, size_t, 0x10 + ((*byteSize) + 0xF) & ~0xF);

	ShaderInfo** const shaderInfos = (ShaderInfo**)ASLR(0xC56863);
	size_t* const shaderInfoCount = (size_t*)ASLR(0xC56868);

	ShaderInfo* newShaderInfos = (ShaderInfo*)operator new(sizeof(ShaderInfo) * (*shaderInfoCount + 1));
	memcpy(newShaderInfos, (char*)*shaderInfos - 4, sizeof(ShaderInfo) * (*shaderInfoCount));

	ShaderInfo& newShaderInfo = newShaderInfos[*shaderInfoCount];
	newShaderInfo.index = ((*byteSize) / 0x10) - 1;
	newShaderInfo.vertexShaderName = "FxFilterT";
	newShaderInfo.pixelShaderName = "FxGammaCorrection";

	WRITE_MEMORY(shaderInfos, char*, (char*)newShaderInfos + 4);
	WRITE_MEMORY(shaderInfoCount, size_t, *shaderInfoCount + 1);
#else // Sacrifice HfSSAO
	WRITE_MEMORY(ASLR(0xEB7DE8), char*, "FxFilterT");
	WRITE_MEMORY(ASLR(0xEB7DEC), char*, "FxGammaCorrection");
#endif

	//
	// Inject DrawInstanceParam
	//
#if 0 // Broken
	gammaCorrectionScreenRenderParamTV.shaderIndex = newShaderInfo.index;
#if 0
	gammaCorrectionScreenRenderParamDRC.shaderIndex = newShaderInfo.index;
#endif
#else
	gammaCorrectionScreenRenderParamTV.shaderIndex = 0x45;
#if 0
	gammaCorrectionScreenRenderParamDRC.shaderIndex = 0x45;
#endif
#endif

	injectDrawInstanceParam((SDrawInstanceParam*)ASLR(0xEBF158), false);
}