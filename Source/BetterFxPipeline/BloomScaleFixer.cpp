#include "BloomScaleFixer.h"
#include "ResolutionScaler.h"

HOOK(void*, __stdcall, InitializeBloomGlareFrameBuffers, 0x10D1C90, YggJob* This)
{
    ResolutionScaler::scaleTo720p = true;

    void* result = originalInitializeBloomGlareFrameBuffers(This);

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
