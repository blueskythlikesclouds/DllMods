#include "D3D9ExUnknown.h"

D3D9ExUnknown::D3D9ExUnknown()
{
    refCount = 1;
}

HRESULT D3D9ExUnknown::QueryInterface(const IID& riid, void** ppvObj)
{
    // Not used by game.
    return S_OK;
}

ULONG D3D9ExUnknown::AddRef()
{
    return ++refCount;
}

ULONG D3D9ExUnknown::Release()
{
    const ULONG result = --refCount;
    if (result == 0)
        delete this;

    return result;
}

D3D9ExUnknown::~D3D9ExUnknown() = default;
