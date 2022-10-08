#pragma once

#include "Shader.h"

class VertexShader : public Shader
{
    mutable std::function<void(VertexShader*)> compileFunc;
    ComPtr<ID3D11VertexShader> vertexShader;
    std::unique_ptr<uint8_t[]> byteCode;
    size_t byteSize;

public:
    VertexShader(std::function<void(VertexShader*)> compileFunc);
    VertexShader(ID3D11Device* device, void* function, size_t functionSize);

    void update(ID3D11Device* device, void* function, size_t functionSize);

    ID3D11VertexShader* getVertexShader() const;

    ID3D11InputLayout* createInputLayout(ID3D11Device* device, const D3D11_INPUT_ELEMENT_DESC* inputElements, size_t inputElementCount) const;
};
