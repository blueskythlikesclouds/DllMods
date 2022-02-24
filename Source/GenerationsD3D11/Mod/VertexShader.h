#pragma once

#include "Unknown.h"

class Device;
class ShaderData;

class VertexShader : public Unknown
{
    ComPtr<ID3D11VertexShader> vertexShader;
    std::unique_ptr<uint8_t[]> byteCode;
    size_t byteSize;

public:
    VertexShader(ID3D11Device* device, const ShaderData& data);

    ID3D11VertexShader* getVertexShader() const;

    ID3D11InputLayout* createInputLayout(ID3D11Device* device, const D3D11_INPUT_ELEMENT_DESC* inputElements, size_t inputElementCount) const;

    virtual HRESULT GetDevice(Device** ppDevice) final;
    virtual HRESULT GetFunction(void*, UINT* pSizeOfData) final;
};
