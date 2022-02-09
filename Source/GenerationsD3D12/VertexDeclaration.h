#pragma once

#include "Unknown.h"

class Device;

class VertexDeclaration : public Unknown
{
    std::unique_ptr<D3D12_INPUT_ELEMENT_DESC[]> elementDescriptions;
    size_t count;

public:
    VertexDeclaration(const D3DVERTEXELEMENT9* vertexElements);

    virtual HRESULT GetDevice(Device** ppDevice);
    virtual HRESULT GetDeclaration(D3DVERTEXELEMENT9* pElement, UINT* pNumElements);
};
