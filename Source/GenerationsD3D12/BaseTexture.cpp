#include "BaseTexture.h"

BaseTexture::BaseTexture(const ComPtr<Device>& d3dDevice, const ComPtr<ID3D12Resource>& d3dResource) : Resource(d3dDevice, d3dResource)
{
}

FUNCTION_STUB(DWORD, BaseTexture::SetLOD, DWORD LODNew)

FUNCTION_STUB(DWORD, BaseTexture::GetLOD)

FUNCTION_STUB(DWORD, BaseTexture::GetLevelCount)

FUNCTION_STUB(HRESULT, BaseTexture::SetAutoGenFilterType, D3DTEXTUREFILTERTYPE FilterType)

FUNCTION_STUB(D3DTEXTUREFILTERTYPE, BaseTexture::GetAutoGenFilterType)

FUNCTION_STUB(void, BaseTexture::GenerateMipSubLevels)

FUNCTION_STUB(void, BaseTexture::FUN_48)

FUNCTION_STUB(HRESULT, BaseTexture::FUN_4C, void* A1)