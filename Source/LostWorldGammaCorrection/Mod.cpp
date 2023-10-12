struct ShaderInfo
{
	size_t index;
	const char* vertexShaderName;
	const char* pixelShaderName;
};

struct DrawInstanceParam
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

HOOK(void, __cdecl, HUDSetupShader, ASLR(0x0052FD60), bool a1, int vs_type, int ps_type, bool set_screen, void* color)
{
	auto infrastructure = app::gfx::RenderManager::GetInstance()->GetRenderingDevice();
	auto& device = infrastructure->m_RenderingDevice;
	static csl::fnd::com_ptr<app::fnd::FileHandleObj> fileHandleObj;

	if (!fileHandleObj)
	{
		fileHandleObj = app::fnd::FileLoader::GetInstance()->LoadFile("SurfRidePixelCorrection.pac", {});
		csl::fnd::Singleton<app::fnd::FileLoader>::GetInstance()->WaitSyncAll();

		auto packFile = app::ObjUtil::GetPackFile("SurfRidePixelCorrection.pac");

		if (packFile.IsValid())
		{
			auto* hudShaders = reinterpret_cast<boost::shared_ptr<hh::mr::CVertexShaderData>*>(ASLR(0x00FDEF08));

			hudShaders[0] = hh::mr::GetMirageVertexShaderDataFromPackfile(packFile, "SurfRide2D");
			hudShaders[1] = hh::mr::GetMirageVertexShaderDataFromPackfile(packFile, "SurfRide3D");
			hudShaders[2] = hh::mr::GetMirageVertexShaderDataFromPackfile(packFile, "SurfRide3DTransform");
		}
	}

	originalHUDSetupShader(a1, vs_type, ps_type, set_screen, color);

	if (device.m_pCurrentVertexShaderData != nullptr)
	{
		hh::rsdx::_RSDXFORMAT format{};
		uint32_t width{};
		uint32_t height{};

		infrastructure->GetScreenInfo(width, height, format);

		const float screen[4] = { static_cast<float>(width), static_cast<float>(height), 0, 0 };
		device.SetVertexShaderParameterF("g_Screen_Size", screen, 0, 1, device.m_pCurrentVertexShaderData);
	}
}

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

DrawInstanceParam gammaCorrectionDrawInstanceParamTV =
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

DrawInstanceParam gammaCorrectionDrawInstanceParamDRC =
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

void injectDrawInstanceParam(DrawInstanceParam* drawInstanceParam, bool drc)
{
	DrawInstanceParam* newDrawInstanceParams = (DrawInstanceParam*)operator new(sizeof(DrawInstanceParam) * (drawInstanceParam->childParamCount + 1));
	memcpy(newDrawInstanceParams, drawInstanceParam->childParams, sizeof(DrawInstanceParam) * drawInstanceParam->childParamCount);

#if 0
	memcpy(&newDrawInstanceParams[drawInstanceParam->childParamCount], drc ? &gammaCorrectionDrawInstanceParamDRC : &gammaCorrectionDrawInstanceParamTV, sizeof(DrawInstanceParam));
#else
	memcpy(&newDrawInstanceParams[drawInstanceParam->childParamCount], &gammaCorrectionDrawInstanceParamTV, sizeof(DrawInstanceParam));
#endif

	WRITE_MEMORY(&drawInstanceParam->childParams, DrawInstanceParam*, newDrawInstanceParams);
	WRITE_MEMORY(&drawInstanceParam->childParamCount, size_t, drawInstanceParam->childParamCount + 1);
}

FUNCTION_PTR(void, __thiscall, fileLoaderLoadFile, ASLR(0x490C80),
	app::fnd::FileLoader* This, csl::fnd::com_ptr<app::fnd::FileHandleObj>& result, const char* pName, const char* a3, const app::fnd::FileLoaderParam& params);

FUNCTION_PTR(void, __thiscall, packFileSetup, ASLR(0xC19560),
	hh::ut::PackFile* This, csl::fnd::IAllocator* allocator, void* renderingInfrastructure);

void* __fastcall getResourceImpl(hh::ut::PackFile* This, void* Edx, const hh::ut::ResourceTypeInfo& info, const char* name, uint* A3)
{
	void* result = This->GetResource(info, name, A3);
	if (result)
		return result;

#if 1
	static csl::fnd::com_ptr<app::fnd::FileHandleObj> fileHandleObj;

	if (!fileHandleObj)
	{
		fileHandleObj = app::fnd::FileLoader::GetInstance()->LoadFile("FxGammaCorrection.pac", {});
		csl::fnd::Singleton<app::fnd::FileLoader>::GetInstance()->WaitSyncAll();
	}

	// Query the pac from ResourceManager because apparently it can just die for some reason
	auto packFile = app::ObjUtil::GetPackFile("FxGammaCorrection.pac");
	if (!packFile.IsValid())
	{
		return nullptr;
	}

	return packFile.GetResource(info, name, A3);
#else
	hh::ut::PackFile packFile(fxGammaCorrectionPacData);

	static bool initialized;
	if (!initialized)
	{
		void* renderingInfrastructure = *(void**)(*(char**)(*(char**)ASLR(0xFD3CC4) + 12) + 180);
		packFileSetup(&packFile, app::fnd::GetTempAllocator(), renderingInfrastructure);

		initialized = true;
	}

	return packFile.GetResource(info, name, A3);
#endif
}

extern "C" void __declspec(dllexport) Init()
{
	// Check if patches were applied already (safety measure for mods that include this DLL)
	if (strcmp(*(char**)ASLR(0xEB802C), "FxGammaCorrection") == 0)
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
#else // Sacrifice FxFXAA_5
	WRITE_MEMORY(ASLR(0xEB8028), char*, "FxFilterT");
	WRITE_MEMORY(ASLR(0xEB802C), char*, "FxGammaCorrection");
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
	gammaCorrectionScreenRenderParamTV.shaderIndex = 0x52;
#if 0
	gammaCorrectionScreenRenderParamDRC.shaderIndex = 0x52;
#endif
#endif

	injectDrawInstanceParam((DrawInstanceParam*)ASLR(0xEBF158), false);

	WRITE_CALL(ASLR(0xC2BF99), getResourceImpl);
	WRITE_CALL(ASLR(0xC2C269), getResourceImpl);

	// SurfRide Half pixel correction
	INSTALL_HOOK(HUDSetupShader);
}