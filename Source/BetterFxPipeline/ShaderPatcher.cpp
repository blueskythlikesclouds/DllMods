#include "Hash.h"
#include "ShaderPatcher.h"
#include "Configuration.h"
#include "MotionBlur.wpu.h"

HOOK(void, __cdecl, VertexShaderCodeMake, 0x734110, 
    const hh::base::CSharedString& name, 
    uint8_t* data, 
    size_t length, 
    const boost::shared_ptr<hh::db::CDatabase>& database, 
    hh::mr::CRenderingInfrastructure* renderingInfrastructure,
    bool delay)
{
    // MakeShadowMapTransparent.wvu
    // Size: 1172 B, CRC32 Hash: 7F8FB70A
    if (length == 1172 && generateCrc32Hash(0, data, length) == 0x7F8FB70A)
    {
        // add o1.xy, c191, v1 -> mov o1.xy, v1
        const uint8_t patch[] =
            { 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x03, 0xE0, 0x01, 0x00, 0xE4, 0x90, 0x00, 0x00, 0x00, 0x00 };

        memcpy(data + 0x474, patch, sizeof(patch));
    }

    return originalVertexShaderCodeMake(name, data, length, database, renderingInfrastructure, delay);
}

HOOK(void, __cdecl, PixelShaderCodeMake, 0x7341B0, 
    const hh::base::CSharedString& name,
    const uint8_t* data,
    size_t length,
    const boost::shared_ptr<hh::db::CDatabase>& database,
    hh::mr::CRenderingInfrastructure* renderingInfrastructure,
    bool delay)
{
    // FxMotionBlur.wpu
    // Size: 1544/1732 B, CRC32 Hash: E81E7AD3/3E11B4C0
    if ((length == 1544 && generateCrc32Hash(0, data, length) == 0xE81E7AD3) ||
        (length == 1732 && generateCrc32Hash(0, data, length) == 0x3E11B4C0))
        return originalPixelShaderCodeMake(name, MOTION_BLUR_PS, sizeof(MOTION_BLUR_PS), database, renderingInfrastructure, delay);

    return originalPixelShaderCodeMake(name, data, length, database, renderingInfrastructure, delay);
}

bool ShaderPatcher::enabled = false;

void ShaderPatcher::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    INSTALL_HOOK(VertexShaderCodeMake);

    if (Configuration::enhancedMotionBlur)
        INSTALL_HOOK(PixelShaderCodeMake);
}
