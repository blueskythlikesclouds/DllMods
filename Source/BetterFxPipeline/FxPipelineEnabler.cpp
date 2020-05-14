#include "FxPipelineEnabler.h"

bool FxPipelineEnabler::enabled = false;

void FxPipelineEnabler::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

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
}
