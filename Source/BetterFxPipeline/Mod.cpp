#include "LostCodeLoader.h"

int32_t internalWidth = -1;
int32_t internalHeight = -1;

uint32_t renderParticles;
uint32_t renderParticleMidAsmHookReturnAddress = 0x10C8278;
uint32_t renderParticleMidAsmHookFunctionAddress = 0x789890;

void __declspec(naked) renderParticleMidAsmHook()
{
    __asm
    {
        cmp renderParticles, 1
        je atTrue

        add esp, 8
        jmp[renderParticleMidAsmHookReturnAddress]

    atTrue:
        call[renderParticleMidAsmHookFunctionAddress]
        jmp[renderParticleMidAsmHookReturnAddress]
    }
}

uint32_t executeRenderJobsMidAsmHookOriginalFunctionAddress = 0x78A2E0;

void __declspec(naked) executeRenderJobsMidAsmHookOriginal()
{
    __asm
    {
        mov eax, [edi]
        mov[esi + 30h], eax
        mov ebx, [edi + 4]
        cmp ebx, [esi + 34h]
        jz loc_78A45A
        test ebx, ebx
        jz loc_78A43D
        lea ecx, [ebx + 8]
        mov edx, 1
        lock xadd[ecx], edx
        
    loc_78A43D:
        mov ecx, [esi + 34h]
        test ecx, ecx
        jz loc_78A457
        lea eax, [ecx + 8]
        or edx, 0FFFFFFFFh
        lock xadd[eax], edx
        jnz loc_78A457
        mov eax, [ecx]
        mov edx, [eax + 8]
        call edx
        
    loc_78A457:
        mov[esi + 34h], ebx
        
    loc_78A45A:
        mov eax, [edi]
        push eax
        mov ecx, esi
        call[executeRenderJobsMidAsmHookOriginalFunctionAddress]
        
        retn
    }
}

uint32_t executeRenderJobsMidAsmHookReturnAddress = 0x78A425;

void __declspec(naked) executeRenderJobsMidAsmHook()
{
    __asm
    {
        push edi
    
        sub edi, [esi + 36]
        
        // Before CFxRenderGameScene
        cmp edi, 40
        je after
        
        // At CFxRenderParticle
        cmp edi, 72
        je at
        
        jmp return
        
    after:
        mov renderParticles, 1
        mov edi, [esi + 36]
        add edi, 72
        call executeRenderJobsMidAsmHookOriginal
        jmp return
        
    at:
        mov renderParticles, 0
        
    return:
        pop edi
        mov eax, [edi]
        mov[esi + 0x30], eax
        jmp[executeRenderJobsMidAsmHookReturnAddress]
    }
}

bool scaleTo720p = false;

void setScales(uint32_t& This, float& widthScale, float& heightScale)
{
    uint32_t width = *(uint32_t*)(This + 200);
    uint32_t height = *(uint32_t*)(This + 204);

    if (scaleTo720p)
    {
        widthScale = 1280.0f / width * widthScale;
        heightScale = 720.0f / height * heightScale;
    }
    else if (internalWidth > 0 && internalHeight > 0)
    {
        widthScale *= internalWidth / (float)width;
        heightScale *= internalHeight / (float)height;
    }
}

HOOK(uint32_t, __fastcall, CreateFrameBuffer1, 0x787980,
    uint32_t This, uint32_t Edx, uint32_t a2, uint32_t a3, float widthScale, float heightScale, uint32_t a6, uint32_t a7)
{
    setScales(This, widthScale, heightScale);
    return originalCreateFrameBuffer1(This, Edx, a2, a3, widthScale, heightScale, a6, a7);
}

HOOK(uint32_t, __fastcall, CreateFrameBuffer2, 0x787A30,
    uint32_t This, uint32_t Edx, uint32_t a2, float widthScale, float heightScale, uint32_t a5, uint32_t a6, uint32_t a7)
{
    setScales(This, widthScale, heightScale);
    return originalCreateFrameBuffer2(This, Edx, a2, widthScale, heightScale, a5, a6, a7);
}

HOOK(uint32_t, __fastcall, CreateFrameBuffer3, 0x7884C0,
    uint32_t This, uint32_t Edx, uint32_t a2, float widthScale, float heightScale, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9)
{
    setScales(This, widthScale, heightScale);
    return originalCreateFrameBuffer3(This, Edx, a2, widthScale, heightScale, a5, a6, a7, a8, a9);
}

HOOK(uint32_t, __fastcall, CreateFrameBuffer4, 0x788590,
    uint32_t This, uint32_t Edx, uint32_t a2, float widthScale, float heightScale, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9)
{
    setScales(This, widthScale, heightScale);
    return originalCreateFrameBuffer4(This, Edx, a2, widthScale, heightScale, a5, a6, a7, a8, a9);
}

HOOK(uint32_t, __stdcall, InitializeBloomGlareFrameBuffers, 0x10D1C90, uint32_t a1)
{
    scaleTo720p = true;

    const uint32_t result = originalInitializeBloomGlareFrameBuffers(a1);

    scaleTo720p = false;

    return result;
}

FUNCTION_PTR(uint32_t, __thiscall, fun7846D0, 0x7846D0, uint32_t This);

HOOK(uint32_t, __fastcall, ExecuteAlternativeDepthOfField, 0x1228960, uint32_t This)
{
    const uint32_t valueAddresses[] = {
        0x01A572E0, 0x01A572E4, 0x01A572E8, 0x01A572EC, 0x01A572F0, 0x01A572F4, 0x01A572F8, 0x01A572FC, 0x01A43594,
        0x01A5730C, 0x01A57304, 0x01A57308, 0x01E5E320
    };
    constexpr size_t valueCount = _countof(valueAddresses);

    const uint32_t obj = fun7846D0(This);
    const uint32_t height = internalHeight > 0 ? internalHeight: *(uint32_t*)(obj + 204);

    uint32_t values[valueCount];
    for (size_t i = 0; i < valueCount; i++)
    {
        values[i] = *(uint32_t*)valueAddresses[i];
        *(float*)valueAddresses[i] *= 720.0f / height;
    }

    const uint32_t result = originalExecuteAlternativeDepthOfField(This);

    for (size_t i = 0; i < valueCount; i++)
        *(uint32_t*)valueAddresses[i] = values[i];

    return result;
}

enum LambertShadow: uint32_t
{
    ENABLE = 0,
    FORCE_DISABLE = 1,
    FORCE_ENABLE = 2
};
LambertShadow lambertShadowObject;
LambertShadow lambertShadowTerrain;

float ambientShadowBiasObject = -1;
float ambientShadowBiasTerrain = -1;

double one = 1.0;

uint32_t renderGameSceneMidAsmHookDisableLambertReturnAddress = 0x10C7E3A;
uint32_t renderGameSceneMidAsmHookEnableLambertReturnAddress = 0x10C8049;

void __declspec(naked) renderGameSceneIsTerrain()
{
    __asm
    {
        push eax
        push ebx

        mov eax, [ebp + 0x08]
        mov eax, [eax]

        mov ebx, [ebp + 0x0C]
        mov ebx, [ebx]

        cmp eax, ebx

        pop ebx
        pop eax

        retn
    }
}

void __declspec(naked) renderGameSceneMidAsmHook()
{
    __asm
    {
        mov edx, 0x1A579A0
        movaps xmm0, [edx]
        movaps[esp + 0x20], xmm0
        xorps xmm1, xmm1
        
        call renderGameSceneIsTerrain
        jz loadTerrainBias

        movss xmm0, ambientShadowBiasObject
        comiss xmm1, xmm0
        jbe main
        
        mov edx, 0x1E5E32C // Ambient_Bias_Shadow_Object
        movss xmm0, [edx]
        jmp main

    loadTerrainBias:
        movss xmm0, ambientShadowBiasTerrain
        comiss xmm1, xmm0
        jbe main

        mov edx, 0x1E5E328 // Ambient_Shadow_Bias_Terrain
        movss xmm0, [edx]

    main:
        movsd xmm1, one
        cvtps2pd xmm0, xmm0
        subsd xmm1, xmm0
        cvtpd2ps xmm0, xmm1
        movss[esp + 0x2C], xmm0
        
        mov eax, [edi]
        mov ecx, [eax + 8]
        movzx edx, byte ptr[ecx + 0x23A]
        cmp edx, 0FFh
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
        call renderGameSceneIsTerrain
        jz moveLambertShadowTerrain
        
        mov edx, lambertShadowObject
        jmp compareLambertShadow
        
    moveLambertShadowTerrain:
        mov edx, lambertShadowTerrain
        
    compareLambertShadow:
        cmp edx, FORCE_DISABLE
        jz disableLambert
        
        cmp edx, FORCE_ENABLE
        jz enableLambert
        
        call renderGameSceneIsTerrain
        jz moveIsEnableLambertShadowTerrain
        
        mov edx, 0x1A4358A // ms_IsEnableLambertShadowObject
        mov al, byte ptr[edx]
        jmp compareIsEnable
        
    moveIsEnableLambertShadowTerrain:
        mov edx, 0x1A43589 // ms_IsEnableLambertShadowTerrain
        mov al, byte ptr[edx]
        
    compareIsEnable:
        cmp al, 1
        jz enableLambert
        
    disableLambert:
        mov edi, [edi]
        mov edx, [edi + 8]
        jmp[renderGameSceneMidAsmHookDisableLambertReturnAddress]
        
    enableLambert:
        mov edi, [edi]
        mov edx, [edi + 8]
        jmp[renderGameSceneMidAsmHookEnableLambertReturnAddress]
    }
}

FUNCTION_PTR(uint32_t, __thiscall, clearAlpha, 0x10D4EB0, void* This);

HOOK(uint32_t, __fastcall, RenderParticles, 0x789890, void* This, void* Edx, uint32_t a2, uint32_t a3)
{
    clearAlpha(This);
    return originalRenderParticles(This, Edx, a2, a3);
}

bool forceIgnoreFinalLightColorAdjustment;
bool forceDirectionalShadows;

extern "C" __declspec(dllexport) void __cdecl OnFrame()
{
    if (forceIgnoreFinalLightColorAdjustment)
        *(bool*)0x1E5E3EA = true;

    if (forceDirectionalShadows)
        *(bool*)0x1A43588 = false;
}

extern "C" __declspec(dllexport) void __cdecl Init(ModInfo * info)
{
    std::string dir = info->CurrentMod->Path;

    size_t pos = dir.find_last_of("\\/");
    if (pos != std::string::npos)
        dir.erase(pos + 1);

    const INIReader reader(dir + "BetterFxPipeline.ini");

    if (reader.ParseError() != 0)
        MessageBox(NULL, L"Failed to parse BetterFxPipeline.ini", NULL, MB_ICONERROR);

    // Swap MTFx to FxPipeline

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

    // Fix black directional shadows
    WRITE_JUMP(0x10C7E35, renderGameSceneMidAsmHook);

    // Fix mrgPlayableParam
    WRITE_MEMORY(0x15CA070, uint8_t, 0x00);

    // Fix g_ForceAlphaColor
    WRITE_MEMORY(0x10D4F62, uint8_t, 0x4C);
    WRITE_MEMORY(0x10D4F68, uint8_t, 0x4C);
    WRITE_MEMORY(0x10D4F6E, uint8_t, 0x4C);
    INSTALL_HOOK(RenderParticles);

    const bool fixBloomScale = reader.GetBoolean("Renderer", "FixBloomScale", true);
    if (fixBloomScale)
        INSTALL_HOOK(InitializeBloomGlareFrameBuffers);

    const bool fixDofScale = reader.GetBoolean("Renderer", "FixDOFScale", true);
    if (fixDofScale)
        INSTALL_HOOK(ExecuteAlternativeDepthOfField);

    forceIgnoreFinalLightColorAdjustment = reader.GetBoolean("Renderer", "ForceIgnoreFinalLightColorAdjustment", false);

    if (reader.GetBoolean("Renderer", "PostProcessingOnParticles", true))
    {
        WRITE_JUMP(0x10C8273, renderParticleMidAsmHook);
        WRITE_JUMP(0x78A420, executeRenderJobsMidAsmHook);
    }

    ambientShadowBiasObject = reader.GetFloat("Shadows", "AmbientShadowBiasObject", -1);
    ambientShadowBiasTerrain = reader.GetFloat("Shadows", "AmbientShadowBiasTerrain", -1);

    if (reader.GetBoolean("Shadows", "ForceCastShadow", false))
    {
        WRITE_MEMORY(0xEBEF87, uint8_t, 0x30, 0xC6, 0x00, 0x01);
        WRITE_NOP(0xEBEF8B, 20);
    }

    forceDirectionalShadows = reader.GetBoolean("Shadows", "ForceDirectionalShadow", true);

    lambertShadowObject = (LambertShadow)reader.GetInteger("Shadows", "LambertShadowObject", ENABLE);
    lambertShadowTerrain = (LambertShadow)reader.GetInteger("Shadows", "LambertShadowTerrain", ENABLE);

    const bool enableScale = reader.GetBoolean("InternalResolution", "Scale", false);
    if (enableScale)
    {
        internalWidth = reader.GetInteger("InternalResolution", "Width", -1);
        internalHeight = reader.GetInteger("InternalResolution", "Height", -1);
    }

    if (fixBloomScale || fixDofScale || enableScale)
    {
        INSTALL_HOOK(CreateFrameBuffer1);
        INSTALL_HOOK(CreateFrameBuffer2);
        INSTALL_HOOK(CreateFrameBuffer3);
        INSTALL_HOOK(CreateFrameBuffer4);
    }
}
