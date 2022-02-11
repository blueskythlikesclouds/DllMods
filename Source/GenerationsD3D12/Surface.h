#pragma once

#include "Device.h"
#include "Resource.h"

class Surface : public Resource
{
public:
    Surface(const ComPtr<Device>& device, const ComPtr<ID3D12Resource>& resource);

    virtual HRESULT GetContainer(const IID& riid, void** ppContainer);
    virtual HRESULT GetDesc(D3DSURFACE_DESC* pDesc);
    virtual HRESULT LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
    virtual HRESULT UnlockRect();
    virtual HRESULT GetDC(HDC* phdc);
    virtual HRESULT ReleaseDC(HDC hdc);
};
