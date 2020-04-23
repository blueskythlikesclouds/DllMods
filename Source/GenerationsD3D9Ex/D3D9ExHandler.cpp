#include "D3D9ExHandler.h"
#include "D3D9Ex.h"

HOOK(D3D9Ex*, __cdecl, Direct3DCreate, 0xA5EDD0, UINT SDKVersion)
{
    return new D3D9Ex(SDKVersion);
}

void D3D9ExHandler::applyPatches()
{
    INSTALL_HOOK(Direct3DCreate);
}
