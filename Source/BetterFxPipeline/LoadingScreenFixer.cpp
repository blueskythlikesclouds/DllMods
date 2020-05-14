#include "LoadingScreenFixer.h"

bool LoadingScreenFixer::enabled = false;

void LoadingScreenFixer::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    // FxPipeline tries to render the loading screen 
    // with MTFx's render director when the thread is busy.
    // Let's not do that.
    WRITE_MEMORY(0xD67E30, uint8_t, 0xC3);
}
