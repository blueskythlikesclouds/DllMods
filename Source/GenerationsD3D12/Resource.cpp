#include "Device.h"
#include "Resource.h"

Resource::Resource(Device* device, ID3D12Resource* resource)
    : device(device), resource(resource)
{
}

Resource::Resource(Device* device, D3D12MA::Allocation* allocation)
    : device(device), resource(allocation ? allocation->GetResource() : nullptr), allocation(allocation)
{
}

Resource::~Resource() = default;

D3D12MA::Allocation* Resource::getAllocation() const
{
    return allocation.Get();
}

ID3D12Resource* Resource::getResource() const
{
    return resource ? resource.Get() :
        allocation ? allocation->GetResource() : nullptr;
}

FUNCTION_STUB(HRESULT, Resource::GetDevice, Device** ppDevice)

FUNCTION_STUB(HRESULT, Resource::SetPrivateData, REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags)

FUNCTION_STUB(HRESULT, Resource::GetPrivateData, REFGUID refguid, void* pData, DWORD* pSizeOfData)

FUNCTION_STUB(HRESULT, Resource::FreePrivateData, REFGUID refguid)

FUNCTION_STUB(DWORD, Resource::SetPriority, DWORD PriorityNew)

FUNCTION_STUB(DWORD, Resource::GetPriority)

FUNCTION_STUB(void, Resource::PreLoad)

FUNCTION_STUB(D3DRESOURCETYPE, Resource::GetType)