#include "VertexDeclaration.h"
#include "TypeConverter.h"

void VertexDeclaration::addIfMissing(LPCSTR semanticName, UINT semanticIndex)
{
    for (auto& element : inputElements)
    {
        if (strcmp(element.SemanticName, semanticName) == 0 && element.SemanticIndex == semanticIndex)
            return;
    }

    auto inputElement = inputElements[0];
    inputElement.SemanticName = semanticName;
    inputElement.SemanticIndex = semanticIndex;
    inputElements.push_back(inputElement);
}

VertexDeclaration::VertexDeclaration(const D3DVERTEXELEMENT9* pVertexElements)
{
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
        desc.SemanticName = TypeConverter::getDeclUsage((D3DDECLUSAGE)pVertexElements[i].Usage);
        desc.SemanticIndex = pVertexElements[i].UsageIndex;
        desc.Format = TypeConverter::getDeclType((D3DDECLTYPE)pVertexElements[i].Type);
        desc.InputSlot = pVertexElements[i].Stream;
        desc.AlignedByteOffset = pVertexElements[i].Offset;
        desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        desc.InstanceDataStepRate = 0;

        inputElements.push_back(desc);
    }

    vertexElements = std::make_unique<D3DVERTEXELEMENT9[]>(inputElements.size() + 1);
    memcpy(vertexElements.get(), pVertexElements, sizeof(D3DVERTEXELEMENT9) * inputElements.size());
    vertexElements[inputElements.size() - 1] = D3DDECL_END();
    vertexElementCount = inputElements.size();

    // TODO: Do this depending on the shader reflection
    addIfMissing("TEXCOORD", 0);
    addIfMissing("TEXCOORD", 1);
    addIfMissing("TEXCOORD", 2);
    addIfMissing("TEXCOORD", 3);
    addIfMissing("BLENDWEIGHT", 0);
    addIfMissing("BLENDINDICES", 0);
}

D3D12_INPUT_LAYOUT_DESC VertexDeclaration::getInputLayoutDesc() const
{
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
    inputLayoutDesc.pInputElementDescs = inputElements.data();
    inputLayoutDesc.NumElements = inputElements.size();
    return inputLayoutDesc;
}

FUNCTION_STUB(HRESULT, VertexDeclaration::GetDevice, Device** ppDevice)

HRESULT VertexDeclaration::GetDeclaration(D3DVERTEXELEMENT9* pElement, UINT* pNumElements)
{
    memcpy(pElement, vertexElements.get(), vertexElementCount * sizeof(D3DVERTEXELEMENT9));
    *pNumElements = vertexElementCount;

    return S_OK;
}