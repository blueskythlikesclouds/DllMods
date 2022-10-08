#pragma once

#include "Shader.h"

class PixelShader : public Shader
{
    mutable std::function<void(PixelShader*)> compileFunc;
    ComPtr<ID3D11PixelShader> pixelShader;

public:
    PixelShader(std::function<void(PixelShader*)> compileFunc);
    PixelShader(ID3D11Device* device, void* function, size_t functionSize);

    void update(ID3D11Device* device, void* function, size_t functionSize);

    ID3D11PixelShader* getPixelShader() const;
};
