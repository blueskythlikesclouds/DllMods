#include "Texture.h"

Texture::Texture(const ComPtr<Device>& d3dDevice, const ComPtr<ID3D12Resource>& d3d12Resource) : BaseTexture(d3dDevice, d3d12Resource)
{
}

FUNCTION_STUB(HRESULT, Texture::GetLevelDesc, UINT Level, D3DSURFACE_DESC *pDesc)

FUNCTION_STUB(HRESULT, Texture::GetSurfaceLevel, UINT Level, Surface** ppSurfaceLevel)

FUNCTION_STUB(HRESULT, Texture::LockRect, UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)

FUNCTION_STUB(HRESULT, Texture::UnlockRect, UINT Level)

FUNCTION_STUB(HRESULT, Texture::AddDirtyRect, CONST RECT* pDirtyRect)