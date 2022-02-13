#include "Resource.h"

#include "Device.h"

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

void Resource::setStates(const D3D12_RESOURCE_STATES initial, const D3D12_RESOURCE_STATES other)
{
    states[0] = initial;
    states[1] = other;
    currentState = initial;
}

void Resource::transition(const D3D12_RESOURCE_STATES before, const D3D12_RESOURCE_STATES after)
{
    ID3D12Resource* resource = getResource();

    if (!resource || currentState != before || currentState == after || ((states[0] != before || states[1] != after) && (states[0] != after || states[1] != before)))
        return;

    device->getCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource, before, after));
    currentState = after;
}

FUNCTION_STUB(HRESULT, Resource::GetDevice, Device** ppDevice)

FUNCTION_STUB(HRESULT, Resource::SetPrivateData, REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags)

FUNCTION_STUB(HRESULT, Resource::GetPrivateData, REFGUID refguid, void* pData, DWORD* pSizeOfData)

FUNCTION_STUB(HRESULT, Resource::FreePrivateData, REFGUID refguid)

FUNCTION_STUB(DWORD, Resource::SetPriority, DWORD PriorityNew)

FUNCTION_STUB(DWORD, Resource::GetPriority)

FUNCTION_STUB(void, Resource::PreLoad)

FUNCTION_STUB(D3DRESOURCETYPE, Resource::GetType)