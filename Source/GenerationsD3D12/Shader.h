#pragma once

#include "Unknown.h"

class Device;

class Shader : public Unknown
{
    std::unique_ptr<uint8_t[]> byteCode;
    size_t byteSize;

public:
    Shader(const void* function, size_t functionSize);

    D3D12_SHADER_BYTECODE getD3DShaderByteCode() const;

    virtual HRESULT GetDevice(Device** ppDevice);
    virtual HRESULT GetFunction(void*, UINT* pSizeOfData);
};