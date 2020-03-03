#include "Hash.h"
#include "TransparentShadowFixer.h"

HOOK(void, __cdecl, LoadVertexShaderCode, 0x734110, uint32_t a1, uint8_t* data, size_t length, void* a4, void* a5,
     void* a6)
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

    return originalLoadVertexShaderCode(a1, data, length, a4, a5, a6);
}

bool TransparentShadowFixer::enabled = false;

void TransparentShadowFixer::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    INSTALL_HOOK(LoadVertexShaderCode);
}
