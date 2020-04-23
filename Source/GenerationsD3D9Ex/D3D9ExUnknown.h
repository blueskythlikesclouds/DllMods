#pragma once

class D3D9ExUnknown
{
protected:
    ULONG refCount;

public:
    D3D9ExUnknown();

    virtual HRESULT QueryInterface(REFIID riid, void** ppvObj);

    virtual ULONG AddRef();

    virtual ULONG Release();

    virtual ~D3D9ExUnknown();
};