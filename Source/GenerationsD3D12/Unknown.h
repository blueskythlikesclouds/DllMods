#pragma once

class Unknown
{
protected:
    ULONG refCount;

public:
    Unknown();

    virtual HRESULT QueryInterface(REFIID riid, void** ppvObj);

    virtual ULONG AddRef();

    virtual ULONG Release();

    virtual ~Unknown();
};