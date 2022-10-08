#include "VertexShader.h"
#include "ShaderCache.h"

VertexShader::VertexShader(std::function<void(VertexShader*)> compileFunc) : compileFunc(compileFunc), byteSize(0)
{
}

VertexShader::VertexShader(ID3D11Device* device, void* function, size_t functionSize)
{
    update(device, function, functionSize);
}

void VertexShader::update(ID3D11Device* device, void* function, const size_t functionSize)
{
    ComPtr<ID3D11VertexShader> shader;
    device->CreateVertexShader(function, functionSize, nullptr, shader.GetAddressOf());

    std::lock_guard lock(ShaderCache::criticalSection);

    vertexShader = std::move(shader);

    byteCode = std::make_unique<uint8_t[]>(functionSize);
    byteSize = functionSize;
    memcpy(byteCode.get(), function, functionSize);
}

ID3D11VertexShader* VertexShader::getVertexShader() const
{
    if (compileFunc)
    {
        ShaderCache::group.run([this, func = std::move(compileFunc)]
        {
            func(const_cast<VertexShader*>(this));
        });
    }

    return vertexShader.Get();
}

ID3D11InputLayout* VertexShader::createInputLayout(ID3D11Device* device, const D3D11_INPUT_ELEMENT_DESC* inputElements, const size_t inputElementCount) const
{
    ID3D11InputLayout* inputLayout = nullptr;
    device->CreateInputLayout(inputElements, inputElementCount, byteCode.get(), byteSize, &inputLayout);

    return inputLayout;
}
