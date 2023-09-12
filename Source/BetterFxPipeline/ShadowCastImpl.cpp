#include "ShadowCastImpl.h"

#include "Configuration.h"

static uint32_t s_metaInstancerAddRenderableMidasmHookRetAddr = 0xCD6875;

static void __declspec(naked) metaInstancerAddRenderableMidAsmHook()
{
    __asm
    {
        mov eax, 0x1E66BEC // Terrain
        mov eax, [eax]
        push ecx
        mov ecx, [ebx + 4]
        jmp[s_metaInstancerAddRenderableMidasmHookRetAddr]
    }
}

void ShadowCastImpl::init()
{
    if (Configuration::enableTerrainShadowCast)
        WRITE_MEMORY(0x13DD294, uint32_t, 0x180);

    if (Configuration::forceCastShadow)
    {
        WRITE_JUMP(0xCD686E, metaInstancerAddRenderableMidAsmHook);

        const auto childParams = reinterpret_cast<Hedgehog::FxRenderFramework::SDrawInstanceParam*>(0x13DC908);
        const auto newChildParams = new Hedgehog::FxRenderFramework::SDrawInstanceParam[4];

        // Object
        newChildParams[0] = childParams[0];
        newChildParams[0].m_Unk0 = 1;

        // Object Overlay
        newChildParams[1] = childParams[0];
        newChildParams[1].m_Unk0 = 6;

        // Player
        newChildParams[2] = childParams[0];
        newChildParams[2].m_Unk0 = 8;

        // Shadow Map Terrain
        newChildParams[3] = childParams[1];

        // Pass new children
        WRITE_MEMORY(0x13DD27C, void*, newChildParams);
        WRITE_MEMORY(0x13DD280, uint32_t, 4);

        WRITE_MEMORY(0x13DD2FC, void*, newChildParams);
        WRITE_MEMORY(0x13DD300, uint32_t, 4);
    }
}
