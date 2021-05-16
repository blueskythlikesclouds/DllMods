#pragma once

class D3D9ExUnknown
{
protected:
    ULONG refCount;

public:
    D3D9ExUnknown();

    virtual HRESULT QueryInterface(REFIID riid, void** ppvObj) final;

    virtual ULONG AddRef() final;

    virtual ULONG Release() final;

    virtual ~D3D9ExUnknown();
};