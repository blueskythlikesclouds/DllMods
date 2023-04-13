#include "ShaderLoader.h"
#include "FxaaRenderer.h"
#include "Configuration.h"

HOOK(void, __stdcall, CApplicationDocumentLoadDatabase, 0xD6A580, Sonic::CApplicationDocument* This)
{
    const hh::base::CSharedString ar = "BetterFxPipeline.ar";
    const hh::base::CSharedString arl = "BetterFxPipeline.arl";

    This->m_pMember->m_spDatabaseLoader->CreateArchiveList(ar, arl, { 200, 5 });
    This->m_pMember->m_spDatabaseLoader->LoadArchiveList(This->m_pMember->m_spShaderDatabase, arl);
    This->m_pMember->m_spDatabaseLoader->LoadArchive(This->m_pMember->m_spShaderDatabase, ar, { -10, 5 }, false, false);

    originalCApplicationDocumentLoadDatabase(This);
}

constexpr size_t SHADER_LIST_BYTE_SIZE = 0x3500;
constexpr size_t SHADER_LIST_EXTRA_BYTE_SIZE = 3 * sizeof(hh::mr::SShaderPair);

static FUNCTION_PTR(void, __thiscall, initializeShader, 0x654480, 
    void* This, int32_t index, hh::db::CDatabase* database, const char* vertexShaderName, const char* pixelShaderName);

HOOK(void, __fastcall, MTFxInitializeShaders, 0x654590, void* This, void* Edx, hh::db::CDatabase* database)
{
    memset((char*)This + SHADER_LIST_BYTE_SIZE, 0, SHADER_LIST_EXTRA_BYTE_SIZE);

    if (Configuration::fxaaIntensity > FxaaIntensity::DISABLED && Configuration::fxaaIntensity <= FxaaIntensity::INTENSITY_6)
        initializeShader(This, 0x350, database, "FxFilterNone", FxaaRenderer::SHADER_NAMES[(size_t)Configuration::fxaaIntensity - 1]);

    initializeShader(This, 0x351, database, "FxFilterT", "LightShaftMask");
    initializeShader(This, 0x352, database, "FxFilterT", "LightShaftFilter");

    originalMTFxInitializeShaders(This, Edx, database);
}

bool ShaderLoader::enabled = false;

void ShaderLoader::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    INSTALL_HOOK(CApplicationDocumentLoadDatabase);

    WRITE_MEMORY(0x6514E3, uint32_t, SHADER_LIST_BYTE_SIZE + SHADER_LIST_EXTRA_BYTE_SIZE);
    INSTALL_HOOK(MTFxInitializeShaders);
}
