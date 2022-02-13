#pragma once

#include "Unknown.h"

class Device;

class VertexDeclaration : public Unknown
{
    std::unique_ptr<D3DVERTEXELEMENT9[]> vertexElements;
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements;
    size_t vertexElementCount{};

    void addIfMissing(LPCSTR semanticName, UINT semanticIndex, DXGI_FORMAT format);

public:
    VertexDeclaration(const D3DVERTEXELEMENT9* vertexElements);

    D3D12_INPUT_LAYOUT_DESC getInputLayoutDesc() const;

    virtual HRESULT GetDevice(Device** ppDevice);
    virtual HRESULT GetDeclaration(D3DVERTEXELEMENT9* pElement, UINT* pNumElements);
};
