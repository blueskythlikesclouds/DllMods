#include "StereoShaderReplacer.h"

bool StereoShaderReplacer::enabled = false;

void StereoShaderReplacer::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    WRITE_MEMORY(0x15CA65F, char, 'r');
    WRITE_MEMORY(0x15CA66B, char, 'r');
    WRITE_MEMORY(0x15CA6CB, char, 'r');
    WRITE_MEMORY(0x15CA6DB, char, 'r');
}
