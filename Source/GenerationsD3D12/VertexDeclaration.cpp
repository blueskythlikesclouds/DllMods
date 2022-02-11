#include "VertexDeclaration.h"
#include "TypeConverter.h"

VertexDeclaration::VertexDeclaration(const D3DVERTEXELEMENT9* vertexElements) : count(0)
{
    for (; vertexElements[count].Stream != 0xFF; count++)
        ;

    d3dVertexElements = std::make_unique<D3DVERTEXELEMENT9[]>(count + 1);
    memcpy(d3dVertexElements.get(), vertexElements, count * sizeof(D3DVERTEXELEMENT9));

    d3dInputElementDescriptions = std::make_unique<D3D12_INPUT_ELEMENT_DESC[]>(count);

    for (size_t i = 0; i < count; i++)
    {
        D3D12_INPUT_ELEMENT_DESC& elementDesc = d3dInputElementDescriptions[i];
        elementDesc.SemanticName = TypeConverter::getDeclUsageName((D3DDECLUSAGE)vertexElements[i].Usage);
        elementDesc.SemanticIndex = vertexElements[i].UsageIndex;
        elementDesc.Format = TypeConverter::getDeclType((D3DDECLTYPE)vertexElements[i].Type);
        elementDesc.InputSlot = vertexElements[i].Stream;
        elementDesc.AlignedByteOffset = vertexElements[i].Offset;
        elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        elementDesc.InstanceDataStepRate = 0;
    }
}

D3D12_INPUT_LAYOUT_DESC VertexDeclaration::getD3DInputLayoutDesc() const
{
    D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
    d3dInputLayoutDesc.pInputElementDescs = d3dInputElementDescriptions.get();
    d3dInputLayoutDesc.NumElements = count;
    return d3dInputLayoutDesc;
}

FUNCTION_STUB(HRESULT, VertexDeclaration::GetDevice, Device** ppDevice)

HRESULT VertexDeclaration::GetDeclaration(D3DVERTEXELEMENT9* pElement, UINT* pNumElements)
{
    memcpy(pElement, d3dVertexElements.get(), count * sizeof(D3DVERTEXELEMENT9));
    *pNumElements = count;

    return S_OK;
}