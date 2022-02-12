#include "BaseTexture.h"

BaseTexture::BaseTexture(Device* device, ID3D12Resource* resource) : Resource(device, resource)
{
}

BaseTexture::BaseTexture(Device* device, D3D12MA::Allocation* allocation) : Resource(device, allocation)
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