#pragma once

#include "Unknown.h"

class Device;
class VertexShader;

class VertexDeclaration : public Unknown
{
    std::unique_ptr<D3DVERTEXELEMENT9[]> vertexElements;
    size_t vertexElementCount{};
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;

    struct InputLayoutHash
    {
        std::size_t operator()(const std::pair<const VertexShader*, bool>& value) const noexcept
        {
            return XXH32(&value, (size_t)&value.second + sizeof(value.second) - (size_t)&value, 0);
        }
    };

    std::unordered_map<std::pair<const VertexShader*, bool>, ComPtr<ID3D11InputLayout>, InputLayoutHash> inputLayouts;

    void addIfNotExist(LPCSTR semanticName, UINT semanticIndex, DXGI_FORMAT format);

public:
    VertexDeclaration(const D3DVERTEXELEMENT9* vertexElements);
    VertexDeclaration(DWORD FVF);

    ID3D11InputLayout* getInputLayout(ID3D11Device* device, const VertexShader* vertexShader, bool instance);

    virtual HRESULT GetDevice(Device** ppDevice);
    virtual HRESULT GetDeclaration(D3DVERTEXELEMENT9* pElement, UINT* pNumElements);
};
