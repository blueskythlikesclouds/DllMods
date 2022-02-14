#include "Resource.h"

#include "Device.h"

Resource::Resource(Device* device, ID3D11Resource* resource)
    : device(device), resource(resource)
{
}

Resource::~Resource() = default;

ID3D11Resource* Resource::getResource() const
{
    return resource.Get();
}

FUNCTION_STUB(HRESULT, Resource::GetDevice, Device** ppDevice)

FUNCTION_STUB(HRESULT, Resource::SetPrivateData, REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags)

FUNCTION_STUB(HRESULT, Resource::GetPrivateData, REFGUID refguid, void* pData, DWORD* pSizeOfData)

FUNCTION_STUB(HRESULT, Resource::FreePrivateData, REFGUID refguid)

FUNCTION_STUB(DWORD, Resource::SetPriority, DWORD PriorityNew)

FUNCTION_STUB(DWORD, Resource::GetPriority)

FUNCTION_STUB(void, Resource::PreLoad)

FUNCTION_STUB(D3DRESOURCETYPE, Resource::GetType)