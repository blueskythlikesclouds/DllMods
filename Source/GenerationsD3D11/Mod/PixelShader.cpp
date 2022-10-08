#include "PixelShader.h"
#include "ShaderCache.h"

PixelShader::PixelShader(std::function<void(PixelShader*)> compileFunc) : compileFunc(compileFunc)
{
}

PixelShader::PixelShader(ID3D11Device* device, void* function, size_t functionSize)
{
    update(device, function, functionSize);
}

void PixelShader::update(ID3D11Device* device, void* function, size_t functionSize)
{
    ComPtr<ID3D11PixelShader> shader;
    device->CreatePixelShader(function, functionSize, nullptr, shader.GetAddressOf());

    std::lock_guard lock(ShaderCache::criticalSection);
    pixelShader = std::move(shader);
}

ID3D11PixelShader* PixelShader::getPixelShader() const
{
    if (compileFunc)
    {
        ShaderCache::group.run([this, func = std::move(compileFunc)]
        {
            func(const_cast<PixelShader*>(this));
        });
    }

    return pixelShader.Get();
}