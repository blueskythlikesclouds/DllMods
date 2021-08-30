#include "Configuration.h"
#include "ShadowCaster.h"

bool ShadowCaster::enabled = false;

void ShadowCaster::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    if (Configuration::enableTerrainShadowCast)
    {
        // FxPipeline
        WRITE_MEMORY(0x10C63F3, uint8_t, 0xC0);
        WRITE_MEMORY(0x10C641D, uint8_t, 0xC0);
        WRITE_MEMORY(0x10C65CC, uint8_t, 0xC0);
        WRITE_MEMORY(0x10C65F6, uint8_t, 0xC0);

        // MTFx
        WRITE_MEMORY(0x13DD294, uint32_t, 0x180);
    }

    if (Configuration::forceCastShadow)
    {
        // FxPipeline
        WRITE_MEMORY(0xEBEF87, uint8_t, 0x30, 0xC6, 0x00, 0x01);
        WRITE_NOP(0xEBEF8B, 20);

        // MTFx
        {
            hh::fx::SDrawInstanceParam* childParams = (hh::fx::SDrawInstanceParam*)0x13DC908;
            hh::fx::SDrawInstanceParam* newChildParams = new hh::fx::SDrawInstanceParam[4];

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
}
