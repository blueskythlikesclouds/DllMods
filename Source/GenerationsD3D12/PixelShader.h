#pragma once

#include "Unknown.h"

class Device;

class PixelShader : public Unknown
{
public:
    virtual HRESULT GetDevice(Device** ppDevice);
    virtual HRESULT GetFunction(void*, UINT* pSizeOfData);
};
