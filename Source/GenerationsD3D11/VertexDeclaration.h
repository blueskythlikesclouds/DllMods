#pragma once

#include "Unknown.h"

class Device;
class VertexShader;

class VertexDeclaration : public Unknown
{
    std::unique_ptr<D3DVERTEXELEMENT9[]> vertexElements;
    size_t vertexElementCount{};
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
    std::map<const VertexShader*, ComPtr<ID3D11InputLayout>> inputLayouts;

    void addIfNotExist(LPCSTR semanticName, UINT semanticIndex, DXGI_FORMAT format);

public:
    VertexDeclaration(ID3D11Device* device, const D3DVERTEXELEMENT9* vertexElements);

    ID3D11InputLayout* getInputLayout(ID3D11Device* device, const VertexShader* vertexShader);

    virtual HRESULT GetDevice(Device** ppDevice);
    virtual HRESULT GetDeclaration(D3DVERTEXELEMENT9* pElement, UINT* pNumElements);
};
