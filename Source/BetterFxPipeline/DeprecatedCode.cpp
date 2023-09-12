#include "DeprecatedCode.h"

#include "AntiAliasing.h"
#include "BloomTypeImpl.h"
#include "Configuration.h"
#include "ParameterManager.h"

// Code here is leftover from old Better FxPipeline when it was
// still using the FxPipeline Renderer.

// All of this will be removed once PBR Shaders mod is ported to MTFx.

static const volatile double ONE = 1.0;

static uint32_t renderGameSceneMidAsmHookDisableLambertReturnAddress = 0x10C7E3A;
static uint32_t renderGameSceneMidAsmHookEnableLambertReturnAddress = 0x10C8049;

static void __declspec(naked) renderGameSceneMidAsmHook()
{
    __asm
    {
        mov edx, 0x1A579A0
        movaps xmm0, [edx]
        movaps[esp + 0x20], xmm0
        xorps xmm1, xmm1

        mov eax, [ebp + 0x08]
        mov eax, [eax]

        mov ebx, [ebp + 0x0C]
        mov ebx, [ebx]

        cmp eax, ebx
        jz loadTerrainBias

        mov edx, 0x1E5E32C // Ambient_Shadow_Bias_Object
        jmp main

    loadTerrainBias:
        mov edx, 0x1E5E328 // Ambient_Shadow_Bias_Terrain

    main:
        movss xmm0, [edx]
        movsd xmm1, ONE
        cvtps2pd xmm0, xmm0
        subsd xmm1, xmm0
        cvtpd2ps xmm0, xmm1
        movss[esp + 0x2C], xmm0

        mov eax, [edi]
        mov ecx, [eax + 8]
        movzx edx, byte ptr[ecx + 0x23A]
        cmp edx, 0xFF
        jz return
        mov ecx, [eax + 4]
        mov eax, [ecx]
        push 1
        lea esi, [esp + 0x24]
        push esi
        push edx
        mov edx, [eax + 0x1B8]
        call edx

    return:
        mov eax, [ebp + 0x08]
        mov eax, [eax]

        mov ebx, [ebp + 0x0C]
        mov ebx, [ebx]

        cmp eax, ebx
        jz moveIsEnableLambertShadowTerrain

        mov edx, 0x1A4358A // ms_IsEnableLambertShadowObject
        jmp compareIsEnable

    moveIsEnableLambertShadowTerrain:
        mov edx, 0x1A43589 // ms_IsEnableLambertShadowTerrain

    compareIsEnable:
        mov al, byte ptr[edx]
        cmp al, 1
        jz enableLambert

        mov edi, [edi]
        mov edx, [edi + 8]
        jmp[renderGameSceneMidAsmHookDisableLambertReturnAddress]

    enableLambert:
        mov edi, [edi]
        mov edx, [edi + 8]
        jmp[renderGameSceneMidAsmHookEnableLambertReturnAddress]
    }
}

static bool scaleTo720p;

static void setScales(hh::ygg::CYggDevice* device, float& widthScale, float& heightScale)
{
    if (scaleTo720p)
    {
        widthScale = 1280.0f / (float)device->m_InternalResolutionWidth * widthScale;
        heightScale = 720.0f / (float)device->m_InternalResolutionHeight * heightScale;
    }
    else if (Configuration::width > 0 && Configuration::height > 0)
    {
        widthScale *= (float)Configuration::width / (float)device->m_InternalResolutionWidth;
        heightScale *= (float)Configuration::height / (float)device->m_InternalResolutionHeight;
    }
}

HOOK(void*, __fastcall, CreateYggTexture1, 0x787980,
    hh::ygg::CYggDevice* This, uint32_t Edx, uint32_t a2, uint32_t a3, float widthScale, float heightScale, uint32_t a6, uint32_t a7)
{
    setScales(This, widthScale, heightScale);
    return originalCreateYggTexture1(This, Edx, a2, a3, widthScale, heightScale, a6, a7);
}

HOOK(void*, __fastcall, CreateYggTexture2, 0x787A30,
    hh::ygg::CYggDevice* This, uint32_t Edx, uint32_t a2, float widthScale, float heightScale, uint32_t a5, uint32_t a6, uint32_t a7)
{
    setScales(This, widthScale, heightScale);
    return originalCreateYggTexture2(This, Edx, a2, widthScale, heightScale, a5, a6, a7);
}

HOOK(void*, __fastcall, CreateYggTexture3, 0x7884C0,
    hh::ygg::CYggDevice* This, uint32_t Edx, uint32_t a2, float widthScale, float heightScale, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9)
{
    setScales(This, widthScale, heightScale);
    return originalCreateYggTexture3(This, Edx, a2, widthScale, heightScale, a5, a6, a7, a8, a9);
}

HOOK(void*, __fastcall, CreateYggTexture4, 0x788590,
    hh::ygg::CYggDevice* This, uint32_t Edx, uint32_t a2, float widthScale, float heightScale, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9)
{
    setScales(This, widthScale, heightScale);
    return originalCreateYggTexture4(This, Edx, a2, widthScale, heightScale, a5, a6, a7, a8, a9);
}

HOOK(void*, __fastcall, SetShader, 0x415EE0, hh::ygg::CYggDevice* This, void* Edx, const boost::shared_ptr<hh::mr::CVertexShaderData>& vertexShaderData, const boost::shared_ptr<hh::mr::CPixelShaderData>& pixelShaderData)
{
    void* result = originalSetShader(This, Edx, vertexShaderData, pixelShaderData);

    const float forceAlphaColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const uint8_t forceAlphaColorRegister = *(uint8_t*)((uint8_t*)This->m_pRenderingInfrastructure->m_RenderingDevice.m_pGlobalParametersData +
        This->m_pRenderingInfrastructure->m_RenderingDevice.m_GlobalParameterIndex * 2 + 544);

    if (forceAlphaColorRegister != 255)
        This->m_pD3DDevice->SetPixelShaderConstantF(forceAlphaColorRegister, forceAlphaColor, 1);

    return result;
}

static FUNCTION_PTR(void*, __thiscall, resetForceAlphaColor, 0x10D4EB0, void* This);

HOOK(void*, __fastcall, RenderMeshes, 0x789890, void* This, void* Edx, void* a2, void* a3)
{
    resetForceAlphaColor(This);
    return originalRenderMeshes(This, Edx, a2, a3);
}

static FUNCTION_PTR(void, __thiscall, CRenderDirectorFxPipelineRender, 0xD1D570, void* This, void* pRenderFramework, void* pRenderData);

HOOK(void, __fastcall, RenderResident, 0x64C900, void* This, void* Edx, void* pRenderFramework, void* pRenderData, void* A3, void* A4)
{
    originalRenderResident(This, Edx, pRenderFramework, pRenderData, A3, A4);
    CRenderDirectorFxPipelineRender((*Sonic::CGameDocument::ms_pInstance)->m_pMember->m_spRenderDirector.get(), pRenderFramework, pRenderData);
}

static hh::mr::SShaderPair fxaaShaderPair;
static boost::shared_ptr<hh::ygg::CYggTexture> fxaaFrameBuffer;

HOOK(void*, __fastcall, InitializeCrossFade, 0x10C21A0, Sonic::CFxJob* This)
{
    fxaaShaderPair = This->m_pScheduler->GetShader("FxFilterNone", AntiAliasing::s_shaderNames[(uint32_t)Configuration::fxaaIntensity - 1]);
    fxaaFrameBuffer = This->m_pScheduler->m_pMisc->m_pDevice->CreateTexture(1.0f, 1.0f, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, NULL);

    return originalInitializeCrossFade(This);
}

HOOK(void*, __fastcall, ExecuteCrossFade, 0x10C22D0, Sonic::CFxJob* This)
{
    void* result = originalExecuteCrossFade(This);

    if (!fxaaShaderPair.m_spVertexShader || !fxaaShaderPair.m_spPixelShader || !fxaaFrameBuffer)
        return result;

    const auto frameBuffer = This->GetDefaultTexture();
    const auto surface = fxaaFrameBuffer->GetSurface(0, 0);

    This->m_pScheduler->m_pMisc->m_pDevice->SetRenderTarget(0, surface);
    This->m_pScheduler->m_pMisc->m_pDevice->SetShader(fxaaShaderPair);

    This->m_pScheduler->m_pMisc->m_pDevice->SetTexture(0, frameBuffer);
    This->m_pScheduler->m_pMisc->m_pDevice->SetSamplerFilter(0, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_NONE);
    This->m_pScheduler->m_pMisc->m_pDevice->SetSamplerAddressMode(0, D3DTADDRESS_CLAMP);

    This->m_pScheduler->m_pMisc->m_pDevice->DrawQuad2D(nullptr, 0, 0);

    This->SetDefaultTexture(fxaaFrameBuffer);

    return result;
}

static hh::mr::SShaderPair bbBloom;
static hh::mr::SShaderPair swaBloom;
static hh::mr::SShaderPair legacyBloom;

static void setBloomShaderPair(hh::mr::SShaderPair& shaderPair)
{
    const BloomType bloomType = BloomTypeImpl::s_bloomType > 0 && BloomTypeImpl::s_bloomType <= (uint32_t)BloomType::FxPipeline + 1 ?
                                    (BloomType)(BloomTypeImpl::s_bloomType - 1) : Configuration::bloomType;

    switch (bloomType)
    {
    case BloomType::MTFx:
        shaderPair = bbBloom;
        break;

    case BloomType::SWA:
        shaderPair = swaBloom;
        break;

    default:
        shaderPair = legacyBloom;
        break;
    }
}

HOOK(void, __fastcall, CFxBloomGlareInitialize, Sonic::fpCFxBloomGlareInitialize, Sonic::CFxBloomGlare* This)
{
    originalCFxBloomGlareInitialize(This);

    bbBloom = This->m_pScheduler->GetShader("RenderBuffer", "HfBloom_BrightPassHDR");
    swaBloom = This->m_pScheduler->GetShader("RenderBuffer", "SWA_Bloom_BrightPassHDR");
    legacyBloom = This->m_pScheduler->GetShader("RenderBuffer", "Bloom_BrightPassHDR");
}

HOOK(void, __fastcall, CFxBloomGlareExecute, Sonic::fpCFxBloomGlareExecute, Sonic::CFxBloomGlare* This)
{
    setBloomShaderPair(This->m_Bloom_BrightPassHDRShader);
    originalCFxBloomGlareExecute(This);
}

HOOK(void*, __stdcall, InitializeBloomGlareFrameBuffers, 0x10D1C90, Sonic::CFxJob* This)
{
    scaleTo720p = true;

    void* result = originalInitializeBloomGlareFrameBuffers(This);

    scaleTo720p = false;

    return result;
}

HOOK(void, __fastcall, ExecuteFxPipelineJobs, 0x78A3D0, void* This, void* Edx, void* A2)
{
    ParameterManager::preUpdate();
    originalExecuteFxPipelineJobs(This, Edx, A2);
    ParameterManager::postUpdate();
}

void DeprecatedCode::init()
{
    if (GetModuleHandle(TEXT("GenerationsPBRShaders.dll")) == nullptr)
        return;

    WRITE_JUMP(0x10C7E35, renderGameSceneMidAsmHook);
    WRITE_MEMORY(0x10C6039, uint32_t, Configuration::shadowResolution);

    if (Configuration::enableTerrainShadowCast)
    {
        // FxPipeline
        WRITE_MEMORY(0x10C63F3, uint8_t, 0xC0);
        WRITE_MEMORY(0x10C641D, uint8_t, 0xC0);
        WRITE_MEMORY(0x10C65CC, uint8_t, 0xC0);
        WRITE_MEMORY(0x10C65F6, uint8_t, 0xC0);
    }

    if (Configuration::forceCastShadow)
    {
        WRITE_MEMORY(0x10C63F2, uint32_t, 0x85);
        WRITE_MEMORY(0x10C641C, uint32_t, 0x85);
        WRITE_MEMORY(0x10C65CB, uint32_t, 0x85);
        WRITE_MEMORY(0x10C65F5, uint32_t, 0x85);
    }

    INSTALL_HOOK(CreateYggTexture1);
    INSTALL_HOOK(CreateYggTexture2);
    INSTALL_HOOK(CreateYggTexture3);
    INSTALL_HOOK(CreateYggTexture4);

    if (Configuration::postProcessingOnParticles)
    {
        // Don't render anything in FxRenderParticle
        WRITE_MEMORY(0x10C8273, uint8_t, 0x83, 0xC4, 0x08, 0x90, 0x90);

        // Render particles in FxRenderGameScene
        WRITE_MEMORY(0x10CAD02, uint8_t, 0x1C);

        // TODO: Fix distortion particles since they don't take water into account. Makes Aquarium Park look wrong.
    }

    // Set g_ForceAlphaColor's default value to white instead of transparent red
    WRITE_MEMORY(0x10D4F62, uint8_t, 0x4C);
    WRITE_MEMORY(0x10D4F68, uint8_t, 0x4C);
    WRITE_MEMORY(0x10D4F6E, uint8_t, 0x4C);

    // Fix mrgPlayableParam's default values
    WRITE_MEMORY(0x16A0D44, float, -10000, 64);
    WRITE_MEMORY(0x16995E8, float, -10000, 64);

    // Initialize g_ForceAlphaColor for every object
    INSTALL_HOOK(SetShader);

    // Initialize g_ForceAlphaColor for every object before rendering
    // because the hook above can't always be trusted apparently
    INSTALL_HOOK(RenderMeshes);

    // FxPipeline tries to render the loading screen 
    // with MTFx's render director when the thread is busy.
    // Let's not do that.
    INSTALL_HOOK(RenderResident);
    WRITE_MEMORY(0x64C6D0, uint8_t, 0xC2, 0x0C, 0x00);

    if (Configuration::fxaaIntensity > FxaaIntensity::Disabled && Configuration::fxaaIntensity <= FxaaIntensity::Intensity6)
    {
        INSTALL_HOOK(InitializeCrossFade);
        INSTALL_HOOK(ExecuteCrossFade);
    }

    // Constructor
    WRITE_MEMORY(0xD1D38F, uint8_t, 0x98, 0x20);
    WRITE_MEMORY(0xD1D395, uint8_t, 0xC0, 0x20);
    WRITE_MEMORY(0xD1D39C, uint8_t, 0xF8, 0x20);
    WRITE_MEMORY(0xD1D3A6, uint8_t, 0x0C);

    // Destructor
    WRITE_MEMORY(0xD1D455, uint8_t, 0x98, 0x20);
    WRITE_MEMORY(0xD1D45B, uint8_t, 0xC0, 0x20);
    WRITE_MEMORY(0xD1D462, uint8_t, 0xF8, 0x20);
    WRITE_MEMORY(0xD1D468, uint8_t, 0x0C);

    INSTALL_HOOK(CFxBloomGlareInitialize);
    INSTALL_HOOK(CFxBloomGlareExecute);

    INSTALL_HOOK(InitializeBloomGlareFrameBuffers);

    INSTALL_HOOK(ExecuteFxPipelineJobs);
}