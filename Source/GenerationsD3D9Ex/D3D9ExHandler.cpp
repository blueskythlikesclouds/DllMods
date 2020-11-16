#include "D3D9ExHandler.h"

#include "D3D9Ex.h"
#include "D3D9ExDevice.h"
#include "D3D9ExVolumeTexture.h"

struct D3DXIMAGE_INFO;

HOOK(HRESULT, __stdcall, D3DXCreateVolumeTextureFromFileInMemoryEx, 0x823FC0,
    IDirect3DDevice9*        pDevice,
    LPCVOID                  pSrcData,
    UINT                     SrcDataSize,
    UINT                     Width,
    UINT                     Height,
    UINT                     Depth,
    UINT                     MipLevels,
    DWORD                    Usage,
    D3DFORMAT                Format,
    D3DPOOL                  Pool,
    DWORD                    Filter,
    DWORD                    MipFilter,
    D3DCOLOR                 ColorKey,
    D3DXIMAGE_INFO* pSrcInfo,
    PALETTEENTRY* pPalette,
    IDirect3DVolumeTexture9** ppVolumeTexture)
{
    D3D9ExDevice* dxpDevice = (D3D9ExDevice*)pDevice;
    IDirect3DVolumeTexture9* d3dVolumeTexture = nullptr;

    HRESULT result = originalD3DXCreateVolumeTextureFromFileInMemoryEx(
        dxpDevice->GetD3DDevice(),
        pSrcData,
        SrcDataSize,
        Width,
        Height,
        Depth,
        MipLevels,
        Usage,
        Format,
        Pool,
        Filter,
        MipFilter,
        ColorKey,
        pSrcInfo,
        pPalette,
        &d3dVolumeTexture
    );

    if (FAILED(result))
    {
        *ppVolumeTexture = nullptr;
        return result;
    }

    *(D3D9ExVolumeTexture**)ppVolumeTexture = new D3D9ExVolumeTexture(dxpDevice, d3dVolumeTexture);
    return result;
}

HOOK(D3D9Ex*, __cdecl, Direct3DCreate, 0xA5EDD0, UINT SDKVersion)
{
    return new D3D9Ex(SDKVersion);
}

void D3D9ExHandler::applyPatches()
{
    INSTALL_HOOK(Direct3DCreate);
    INSTALL_HOOK(D3DXCreateVolumeTextureFromFileInMemoryEx);
}
