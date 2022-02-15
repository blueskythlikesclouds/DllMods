#include "VertexDeclaration.h"
#include "TypeConverter.h"
#include "VertexShader.h"

void VertexDeclaration::addIfNotExist(LPCSTR semanticName, UINT semanticIndex, DXGI_FORMAT format)
{
    for (auto& element : inputElements)
    {
        if (strcmp(element.SemanticName, semanticName) == 0 && element.SemanticIndex == semanticIndex)
            return;
    }

    auto inputElement = inputElements[0];
    inputElement.SemanticName = semanticName;
    inputElement.SemanticIndex = semanticIndex;
    inputElement.Format = format;

    inputElements.push_back(inputElement);
}

VertexDeclaration::VertexDeclaration(const D3DVERTEXELEMENT9* pVertexElements)
{
    for (int i = 0; ; i++)
    {
        if (pVertexElements[i].Stream == 0xFF)
            break;

        D3D11_INPUT_ELEMENT_DESC desc;
        desc.SemanticName = TypeConverter::getDeclUsage((D3DDECLUSAGE)pVertexElements[i].Usage);
        desc.SemanticIndex = pVertexElements[i].UsageIndex;
        desc.Format = TypeConverter::getDeclType((D3DDECLTYPE)pVertexElements[i].Type);
        desc.InputSlot = pVertexElements[i].Stream;
        desc.AlignedByteOffset = pVertexElements[i].Offset;
        desc.InputSlotClass = pVertexElements[i].Stream >= 1 ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
        desc.InstanceDataStepRate = pVertexElements[i].Stream >= 1 ? 1 : 0;

        inputElements.push_back(desc);
    }

    vertexElements = std::make_unique<D3DVERTEXELEMENT9[]>(inputElements.size() + 1);
    memcpy(vertexElements.get(), pVertexElements, sizeof(D3DVERTEXELEMENT9) * inputElements.size());
    vertexElements[inputElements.size()] = D3DDECL_END();
    vertexElementCount = inputElements.size();

    // TODO: Do this depending on the shader reflection
    addIfNotExist("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    addIfNotExist("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    addIfNotExist("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    addIfNotExist("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    addIfNotExist("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
    addIfNotExist("TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT);
    addIfNotExist("TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT);
    addIfNotExist("TEXCOORD", 3, DXGI_FORMAT_R32G32_FLOAT);
    addIfNotExist("COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM);
    addIfNotExist("BLENDWEIGHT", 0, DXGI_FORMAT_R8G8B8A8_UNORM);
    addIfNotExist("BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT);
}

VertexDeclaration::VertexDeclaration(DWORD FVF)
{
    size_t offset = 0;

    if (FVF & D3DFVF_XYZRHW)
    {
        inputElements.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
        offset += 16;
    }

    if (FVF & D3DFVF_DIFFUSE)
    {
        inputElements.push_back({ "COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
        offset += 4;
    }

    if (FVF & D3DFVF_TEX1)
    {
        inputElements.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
        offset += 8;
    }
}

ID3D11InputLayout* VertexDeclaration::getInputLayout(ID3D11Device* device, const VertexShader* vertexShader)
{
    const auto pair = inputLayouts.find(vertexShader);
    if (pair != inputLayouts.end())
        return pair->second.Get();

    ID3D11InputLayout* inputLayout = vertexShader->createInputLayout(device, inputElements.data(), inputElements.size());
    inputLayouts.insert(std::make_pair(vertexShader, inputLayout));

    return inputLayout;
}

FUNCTION_STUB(HRESULT, VertexDeclaration::GetDevice, Device** ppDevice)

HRESULT VertexDeclaration::GetDeclaration(D3DVERTEXELEMENT9* pElement, UINT* pNumElements)
{
    memcpy(pElement, vertexElements.get(), (vertexElementCount + 1) * sizeof(D3DVERTEXELEMENT9));
    *pNumElements = vertexElementCount + 1;

    return S_OK;
}