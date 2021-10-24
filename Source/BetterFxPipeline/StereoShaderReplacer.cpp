#include "StereoShaderReplacer.h"

bool StereoShaderReplacer::enabled = false;

void StereoShaderReplacer::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    WRITE_MEMORY(0x11C965F, char, 'r');
    WRITE_MEMORY(0x11C966B, char, 'r');
    WRITE_MEMORY(0x11C96CB, char, 'r');
    WRITE_MEMORY(0x11C96DB, char, 'r');
}
