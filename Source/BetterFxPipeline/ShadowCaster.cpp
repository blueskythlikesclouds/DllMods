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
        WRITE_MEMORY(0x10C63F3, uint8_t, 0xC0);
        WRITE_MEMORY(0x10C641D, uint8_t, 0xC0);
        WRITE_MEMORY(0x10C65CC, uint8_t, 0xC0);
        WRITE_MEMORY(0x10C65F6, uint8_t, 0xC0);
    }

    if (Configuration::forceCastShadow)
    {
        WRITE_MEMORY(0xEBEF87, uint8_t, 0x30, 0xC6, 0x00, 0x01);
        WRITE_NOP(0xEBEF8B, 20);
    }
}
