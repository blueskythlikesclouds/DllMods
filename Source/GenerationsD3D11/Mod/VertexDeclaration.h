#pragma once

#include "Unknown.h"

class Device;
class VertexShader;

class VertexDeclaration : public Unknown
{
    std::unique_ptr<D3DVERTEXELEMENT9[]> vertexElements;
    size_t vertexElementCount{};
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;

    bool hasBone{};
    bool has10BitNormal{};
    bool hasBinormal{};
    bool isFVF;

    std::unordered_map<size_t, ComPtr<ID3D11InputLayout>> inputLayouts;

    void addIfNotExist(LPCSTR semanticName, UINT semanticIndex, DXGI_FORMAT format);

public:
    VertexDeclaration(const D3DVERTEXELEMENT9* vertexElements);
    VertexDeclaration(DWORD FVF);

    bool getHasBone() const;
    bool getHas10BitNormal() const;
    bool getHasBinormal() const;
    bool getIsFVF() const;

    ID3D11InputLayout* getInputLayout(ID3D11Device* device, const VertexShader* vertexShader, bool instance);

    virtual HRESULT GetDevice(Device** ppDevice) final;
    virtual HRESULT GetDeclaration(D3DVERTEXELEMENT9* pElement, UINT* pNumElements) final;
};