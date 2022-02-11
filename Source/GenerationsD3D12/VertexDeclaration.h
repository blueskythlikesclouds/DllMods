#pragma once

#include "Unknown.h"

class Device;

class VertexDeclaration : public Unknown
{
    std::unique_ptr<D3DVERTEXELEMENT9[]> vertexElements;
    std::unique_ptr<D3D12_INPUT_ELEMENT_DESC[]> inputElements;
    size_t count;

public:
    VertexDeclaration(const D3DVERTEXELEMENT9* vertexElements);

    D3D12_INPUT_LAYOUT_DESC getInputLayoutDesc() const;

    virtual HRESULT GetDevice(Device** ppDevice);
    virtual HRESULT GetDeclaration(D3DVERTEXELEMENT9* pElement, UINT* pNumElements);
};
