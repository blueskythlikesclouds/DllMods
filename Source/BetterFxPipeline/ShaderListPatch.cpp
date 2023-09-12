#include "ShaderListPatch.h"

#include "AntiAliasing.h"
#include "Configuration.h"

static constexpr size_t SHADER_LIST_BYTE_SIZE = 0x3500;
static constexpr size_t SHADER_LIST_EXTRA_BYTE_SIZE = 3 * sizeof(Hedgehog::Mirage::SShaderPair);

static FUNCTION_PTR(void, __thiscall, makeShaderPair, 0x654480,
    void* This, int32_t index, Hedgehog::Database::CDatabase* database, const char* vertexShaderName, const char* pixelShaderName);

HOOK(void, __fastcall, MakeShaderList, 0x654590, void* This, void* _, Hedgehog::Database::CDatabase* database)
{
    memset(static_cast<uint8_t*>(This) + SHADER_LIST_BYTE_SIZE, 0, SHADER_LIST_EXTRA_BYTE_SIZE);

    if (Configuration::fxaaIntensity > FxaaIntensity::Disabled && Configuration::fxaaIntensity <= FxaaIntensity::Intensity6)
        makeShaderPair(This, 0x350, database, "FxFilterNone", AntiAliasing::s_shaderNames[static_cast<size_t>(Configuration::fxaaIntensity) - 1]);

    makeShaderPair(This, 0x351, database, "FxFilterT", "LightShaftMask");
    makeShaderPair(This, 0x352, database, "FxFilterT", "LightShaftFilter");

    originalMakeShaderList(This, _, database);
}

HOOK(void, __stdcall, ApplicationDocumentLoadDatabase, 0xD6A580, Sonic::CApplicationDocument* This)
{
    const hh::base::CSharedString ar = "BetterFxPipeline.ar";
    const hh::base::CSharedString arl = "BetterFxPipeline.arl";

    This->m_pMember->m_spDatabaseLoader->CreateArchiveList(ar, arl, { 200, 5 });
    This->m_pMember->m_spDatabaseLoader->LoadArchiveList(This->m_pMember->m_spShaderDatabase, arl);
    This->m_pMember->m_spDatabaseLoader->LoadArchive(This->m_pMember->m_spShaderDatabase, ar, { -10, 5 }, false, false);

    originalApplicationDocumentLoadDatabase(This);
}

void ShaderListPatch::init()
{
    WRITE_MEMORY(0x6514E3, uint32_t, SHADER_LIST_BYTE_SIZE + SHADER_LIST_EXTRA_BYTE_SIZE);
    INSTALL_HOOK(MakeShaderList);
    INSTALL_HOOK(ApplicationDocumentLoadDatabase);
}
