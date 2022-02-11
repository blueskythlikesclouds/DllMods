#include "Texture.h"

Texture::Texture(const ComPtr<Device>& device, const ComPtr<ID3D12Resource>& resource) : BaseTexture(device, resource)
{
}

FUNCTION_STUB(HRESULT, Texture::GetLevelDesc, UINT Level, D3DSURFACE_DESC *pDesc)

FUNCTION_STUB(HRESULT, Texture::GetSurfaceLevel, UINT Level, Surface** ppSurfaceLevel)

FUNCTION_STUB(HRESULT, Texture::LockRect, UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)

FUNCTION_STUB(HRESULT, Texture::UnlockRect, UINT Level)

FUNCTION_STUB(HRESULT, Texture::AddDirtyRect, CONST RECT* pDirtyRect)