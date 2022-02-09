#pragma once

#include "Unknown.h"

class Device;

class VertexShader : public Unknown
{
public:
    virtual HRESULT GetDevice(Device** ppDevice);
    virtual HRESULT GetFunction(void*, UINT* pSizeOfData);
};