HOOK(void, __fastcall, CFxRenderGameSceneExecute, Sonic::fpCFxRenderGameSceneExecute, Sonic::CFxRenderGameScene* This)
{
    boost::shared_ptr<Hedgehog::Yggdrasill::CYggTexture> spShadowMapTex;
    This->GetTexture(spShadowMapTex, "shadowmap");

    float shadowMapSize[] = { spShadowMapTex->m_CreationParams.Width, 1.0f / spShadowMapTex->m_CreationParams.Width, 0, 0 };
    This->m_pScheduler->m_pMisc->m_pDevice->m_pD3DDevice->SetPixelShaderConstantF(223, shadowMapSize, 1);

    originalCFxRenderGameSceneExecute(This);
}

HOOK(uint32_t, __fastcall, CRenderingDeviceSetAtlasParameterData, Hedgehog::Mirage::fpCRenderingDeviceSetAtlasParameterData, 
    Hedgehog::Mirage::CRenderingDevice* This, void* Edx, float* const pData)
{
    if (pData == (float*)0x13DEAB0) // Fail-safe for non-existent GI textures
        return originalCRenderingDeviceSetAtlasParameterData(This, Edx, pData);

    DX_PATCH::IDirect3DTexture9* dxpTexture = *(DX_PATCH::IDirect3DTexture9**)((uint32_t)pData + 16);

    D3DSURFACE_DESC desc;
    dxpTexture->GetLevelDesc(0, &desc);

    float giAtlasSize[] = { (float)desc.Width, (float)desc.Height, 1.0f / (float)desc.Width, 1.0f / (float)desc.Height };
    This->m_pD3DDevice->SetPixelShaderConstantF(222, giAtlasSize, 1);

    return originalCRenderingDeviceSetAtlasParameterData(This, Edx, pData);
}

constexpr uint32_t RAWZ = MAKEFOURCC('R', 'A', 'W', 'Z');

uint32_t CFxShadowMapInitializeMidAsmHookReturnAddress = 0x10C60B0;

void __declspec(naked) CFxShadowMapInitializeMidAsmHook()
{
    __asm
    {
        push RAWZ // RAWZ instead of D24S8 so GPU doesn't do depth comparison
        push 2
        push 1
        jmp[CFxShadowMapInitializeMidAsmHookReturnAddress]
    }
}

extern "C" __declspec(dllexport) void OnFrame()
{
    *(float*)0x1A4356C = 0; // Force shadow bias to 0
}

extern "C" __declspec(dllexport) void Init()
{
    INSTALL_HOOK(CFxRenderGameSceneExecute);
    INSTALL_HOOK(CRenderingDeviceSetAtlasParameterData);

    WRITE_JUMP(0x10C60AA, CFxShadowMapInitializeMidAsmHook);

    // Linear -> Point
    WRITE_MEMORY(0x10C7D6B, uint8_t, 0x00);
    WRITE_MEMORY(0x10C7D6B + 2, uint8_t, 0x01);
    WRITE_MEMORY(0x10C7D6B + 4, uint8_t, 0x01);

    WRITE_MEMORY(0x10C7DC2, uint8_t, 0x00);
    WRITE_MEMORY(0x10C7DC2 + 2, uint8_t, 0x01);
    WRITE_MEMORY(0x10C7DC2 + 4, uint8_t, 0x01);

    WRITE_MEMORY(0x10C7EA9, uint8_t, 0x00);
    WRITE_MEMORY(0x10C7EA9 + 2, uint8_t, 0x01);
    WRITE_MEMORY(0x10C7EA9 + 4, uint8_t, 0x01);

    WRITE_MEMORY(0x10C7F00, uint8_t, 0x00);
    WRITE_MEMORY(0x10C7F00 + 2, uint8_t, 0x01);
    WRITE_MEMORY(0x10C7F00 + 4, uint8_t, 0x01);
}

extern "C" __declspec(dllexport) void PostInit()
{
    if (GetModuleHandle(TEXT("BetterFxPipeline.dll")) != nullptr)
        return;

    MessageBox(nullptr, TEXT("This mod requires the latest version of Better FxPipeline enabled."), TEXT("Soft Shadows"), MB_ICONERROR);
    exit(-1);
}