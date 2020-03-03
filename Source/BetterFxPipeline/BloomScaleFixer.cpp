#include "BloomScaleFixer.h"
#include "ResolutionScaler.h"

HOOK(uint32_t, __stdcall, InitializeBloomGlareFrameBuffers, 0x10D1C90, uint32_t a1)
{
    ResolutionScaler::scaleTo720p = true;

    const uint32_t result = originalInitializeBloomGlareFrameBuffers(a1);

    ResolutionScaler::scaleTo720p = false;

    return result;
}

bool BloomScaleFixer::enabled = false;

void BloomScaleFixer::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    ResolutionScaler::applyPatches();

    INSTALL_HOOK(InitializeBloomGlareFrameBuffers);
}
