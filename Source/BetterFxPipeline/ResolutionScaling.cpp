#include "ResolutionScaling.h"

#include "Configuration.h"

HOOK(void, __stdcall, InitializeSurfaceInfo, 0x653A00, void* A1, uint32_t* A2, void* A3)
{
    originalInitializeSurfaceInfo(A1, A2, A3);

    if (Configuration::enableResolutionScale && 
        Configuration::width > 0 && Configuration::height > 0)
    {
        // Planar reflection
        A2[2] = A2[2] * Configuration::width / *reinterpret_cast<uint32_t*>(0x1DFDDDC);
        A2[3] = A2[3] * Configuration::height / *reinterpret_cast<uint32_t*>(0x1DFDDE0);

        // Framebuffer resolution
        A2[4] = Configuration::width;
        A2[5] = Configuration::height;

        // Viewport
        A2[6] = Configuration::width;
        A2[7] = Configuration::height;
    }

    // Shadow
    A2[8] = Configuration::shadowResolution;
    A2[9] = Configuration::shadowResolution;
}

void ResolutionScaling::init()
{
    INSTALL_HOOK(InitializeSurfaceInfo);
}
