#include "ShaderCodePatch.h"

#include "Configuration.h"
#include "MotionBlur.wpu.h"

// add o1.xy, c191, v1 -> mov o1.xy, v1
static const uint8_t s_patch[] =
    { 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x03, 0xE0, 0x01, 0x00, 0xE4, 0x90, 0x00, 0x00, 0x00, 0x00 };

HOOK(void, __cdecl, VertexShaderCodeMake, 0x734110,
     const Hedgehog::Base::CSharedString& name,
     uint8_t* data,
     size_t length,
     const boost::shared_ptr<Hedgehog::Database::CDatabase>& database,
     Hedgehog::Mirage::CRenderingInfrastructure* renderingInfrastructure,
     bool delay)
{
    // MakeShadowMapTransparent.wvu
    if (length == 1172)
    {
        const XXH32_hash_t hash = XXH32(data, length, 0);

        if (hash == 0xA5C48D5)
            memcpy(data + 0x474, s_patch, sizeof(s_patch));

        else if (hash == 0x343EBC2D)
            memcpy(data + 0x1E4, s_patch, sizeof(s_patch));
    }

    return originalVertexShaderCodeMake(name, data, length, database, renderingInfrastructure, delay);
}

HOOK(void, __cdecl, PixelShaderCodeMake, 0x7341B0,
    const Hedgehog::Base::CSharedString& name,
    const uint8_t* data,
    size_t length,
    const boost::shared_ptr<Hedgehog::Database::CDatabase>& database,
    Hedgehog::Mirage::CRenderingInfrastructure* renderingInfrastructure,
    bool delay)
{
    if ((Configuration::enhancedMotionBlur && (length == 1544 || length == 1732)) || length == 2640)
    {
        const XXH32_hash_t hash = XXH32(data, length, 0);

        if (Configuration::enhancedMotionBlur && ((length == 1544 && hash == 0xDAF93F45) || (length == 1732 && hash == 0xEC6513C7)))
            return originalPixelShaderCodeMake(name, MOTION_BLUR_PS, sizeof(MOTION_BLUR_PS), database, renderingInfrastructure, delay);

        // Disallow the Unleashed FxPipeline Shaders mod as it is not required and causes graphical issues.
        if (length == 2640 && hash == 0x7EDFE3DC)
        {
            MessageBox(
                nullptr, 
                TEXT("Better FxPipeline is incompatible with the \"Unleashed FxPipeline Shaders\" mod. Please disable it in order to use this mod."), 
                TEXT("Better FxPipeline"),
                MB_ICONERROR);

            exit(-1);
        }
    }

    return originalPixelShaderCodeMake(name, data, length, database, renderingInfrastructure, delay);
}

void ShaderCodePatch::init()
{
    INSTALL_HOOK(VertexShaderCodeMake);
    INSTALL_HOOK(PixelShaderCodeMake);

    // Stereo -> Regular
    WRITE_MEMORY(0x15CA65F, char, 'r');
    WRITE_MEMORY(0x15CA66B, char, 'r');
    WRITE_MEMORY(0x15CA6CB, char, 'r');
    WRITE_MEMORY(0x15CA6DB, char, 'r');
}
