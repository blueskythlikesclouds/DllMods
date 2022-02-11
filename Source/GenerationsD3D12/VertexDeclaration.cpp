#include "VertexDeclaration.h"
#include "TypeConverter.h"

VertexDeclaration::VertexDeclaration(const D3DVERTEXELEMENT9* pVertexElements) : count(0)
{
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementList;

    for (int i = 0; ; i++)
    {
        if (pVertexElements[i].Stream == 0xFF)
            break;

        // Sloppily validate declaration, some declarations passed by game don't have D3DDECL_END()
        bool stop = false;

        for (int j = 0; j < i; j++)
            stop |= pVertexElements[i].Usage == pVertexElements[j].Usage && pVertexElements[i].UsageIndex == pVertexElements[j].UsageIndex;

        if (stop)
            break;

        D3D12_INPUT_ELEMENT_DESC desc;
        desc.SemanticName = TypeConverter::getDeclUsageName((D3DDECLUSAGE)pVertexElements[i].Usage);
        desc.SemanticIndex = pVertexElements[i].UsageIndex;
        desc.Format = TypeConverter::getDeclType((D3DDECLTYPE)pVertexElements[i].Type);
        desc.InputSlot = pVertexElements[i].Stream;
        desc.AlignedByteOffset = pVertexElements[i].Offset;
        desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        desc.InstanceDataStepRate = 0;

        inputElementList.push_back(desc);
    }

    vertexElements = std::make_unique<D3DVERTEXELEMENT9[]>(inputElementList.size() + 1);
    memcpy(vertexElements.get(), pVertexElements, sizeof(D3DVERTEXELEMENT9) * inputElementList.size());
    vertexElements[inputElementList.size() - 1] = D3DDECL_END();

    inputElements = std::make_unique<D3D12_INPUT_ELEMENT_DESC[]>(inputElementList.size());
    memcpy(inputElements.get(), inputElementList.data(), sizeof(D3D12_INPUT_ELEMENT_DESC) * inputElementList.size());

    count = inputElementList.size();
}

D3D12_INPUT_LAYOUT_DESC VertexDeclaration::getInputLayoutDesc() const
{
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
    inputLayoutDesc.pInputElementDescs = inputElements.get();
    inputLayoutDesc.NumElements = count;
    return inputLayoutDesc;
}

FUNCTION_STUB(HRESULT, VertexDeclaration::GetDevice, Device** ppDevice)

HRESULT VertexDeclaration::GetDeclaration(D3DVERTEXELEMENT9* pElement, UINT* pNumElements)
{
    memcpy(pElement, vertexElements.get(), count * sizeof(D3DVERTEXELEMENT9));
    *pNumElements = count;

    return S_OK;
}