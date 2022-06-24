std::unique_ptr<uint8_t[]> shader;
size_t shaderSize;

void error(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char errorMsg[0x400];
    vsprintf_s(errorMsg, format, args);

    MessageBoxA(nullptr, errorMsg, "OriginsPixelFilter", MB_ICONERROR);
}

VTABLE_HOOK(HRESULT, WINAPI, ID3D11Device, CreatePixelShader,
            const void* pShaderBytecode,
            SIZE_T BytecodeLength,
            ID3D11ClassLinkage* pClassLinkage,
            ID3D11PixelShader** ppPixelShader
)
{
    if (BytecodeLength == 0xA28 && XXH64(pShaderBytecode, BytecodeLength, 0) == 0x2E7DE3D786C4A4F3)
    {
        const HRESULT result = originalID3D11DeviceCreatePixelShader(This, shader.get(), shaderSize, pClassLinkage, ppPixelShader);
        if (FAILED(result))
            error("Failed to create pixel shader.");

        return result;
    }

	return originalID3D11DeviceCreatePixelShader(This, pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
}

HOOK(HRESULT, WINAPI, D3D11CreateDeviceImp, PROC_ADDRESS("d3d11.dll", "D3D11CreateDevice"),
     IDXGIAdapter* pAdapter,
     D3D_DRIVER_TYPE DriverType,
     HMODULE Software,
     UINT Flags,
     const D3D_FEATURE_LEVEL* pFeatureLevels,
     UINT FeatureLevels,
     UINT SDKVersion,
     ID3D11Device** ppDevice,
     D3D_FEATURE_LEVEL* pFeatureLevel,
     ID3D11DeviceContext** ppImmediateContext
)
{
	const HRESULT result = originalD3D11CreateDeviceImp(
        pAdapter, 
        DriverType,
        Software,
        Flags, 
        pFeatureLevels,
        FeatureLevels,
        SDKVersion, 
        ppDevice,
        pFeatureLevel,
        ppImmediateContext);

	if (SUCCEEDED(result) && *ppDevice)
		INSTALL_VTABLE_HOOK(ID3D11Device, *ppDevice, CreatePixelShader, 15);

	return result;
}

extern "C" __declspec(dllexport) void Init()
{
    const INIReader reader("OriginsPixelFilter.ini");
    if (reader.ParseError() != 0)
        return error("Failed to parse \"OriginsPixelFilter.ini\".");

    const std::string shaderFileName = reader.Get("Main", "ShaderFileName", std::string());

    FILE* file = fopen(shaderFileName.c_str(), "rb");
    if (!file)
        return error("Failed to parse \"%s\".", shaderFileName.c_str());

    fseek(file, 0, SEEK_END);
    shaderSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    shader = std::make_unique<uint8_t[]>(shaderSize);
    fread(shader.get(), 1, shaderSize, file);
    fclose(file);

    if (shaderSize < 4 || *(uint32_t*)shader.get() != MAKEFOURCC('D', 'X', 'B', 'C')) // DXBC
    {
        ComPtr<ID3DBlob> code;
        ComPtr<ID3DBlob> errorMsg;

        const HRESULT result = D3DCompile(
            shader.get(),
            shaderSize, 
            shaderFileName.c_str(),
            nullptr, 
            nullptr,
            "main",
            "ps_5_0",
            0, 
            0,
            code.GetAddressOf(), 
            errorMsg.GetAddressOf());

        if (!code || FAILED(result))
        {
            if (errorMsg && errorMsg->GetBufferSize() > 0)
                return error("Failed to compile \"%s\".\n\nReason:\n%s", shaderFileName.c_str(), errorMsg->GetBufferPointer());

            return error("Failed to compile \"%s\".", shaderFileName.c_str());
        }

        shaderSize = code->GetBufferSize();
        shader = std::make_unique<uint8_t[]>(shaderSize);
        memcpy(shader.get(), code->GetBufferPointer(), shaderSize);
    }

    INSTALL_HOOK(D3D11CreateDeviceImp);
}