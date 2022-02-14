#include "VertexShader.h"

VertexShader::VertexShader(ID3D11Device* device, const void* function, size_t functionSize)
    : byteCode(std::make_unique<uint8_t[]>(functionSize)), byteSize(functionSize)
{
    memcpy(byteCode.get(), function, functionSize);
    device->CreateVertexShader(function, functionSize, nullptr, vertexShader.GetAddressOf());
}

ID3D11VertexShader* VertexShader::getVertexShader() const
{
    return vertexShader.Get();
}

ID3D11InputLayout* VertexShader::createInputLayout(ID3D11Device* device, const D3D11_INPUT_ELEMENT_DESC* inputElements, const size_t inputElementCount) const
{
    ID3D11InputLayout* inputLayout = nullptr;
    device->CreateInputLayout(inputElements, inputElementCount, byteCode.get(), byteSize, &inputLayout);

    return inputLayout;
}

FUNCTION_STUB(HRESULT, VertexShader::GetDevice, Device** ppDevice)

FUNCTION_STUB(HRESULT, VertexShader::GetFunction, void*, UINT* pSizeOfData)

