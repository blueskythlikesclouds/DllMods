#include "Shader.h"

Shader::Shader(const void* function, const size_t functionSize)
    : byteCode(std::make_unique<uint8_t[]>(functionSize)), byteSize(functionSize)
{
    memcpy(byteCode.get(), function, functionSize);
}

D3D12_SHADER_BYTECODE Shader::getShaderByteCode() const
{
    D3D12_SHADER_BYTECODE shaderByteCode;
    shaderByteCode.pShaderBytecode = byteCode.get();
    shaderByteCode.BytecodeLength = byteSize;
    return shaderByteCode;
}

FUNCTION_STUB(HRESULT, Shader::GetDevice, Device** ppDevice)

FUNCTION_STUB(HRESULT, Shader::GetFunction, void*, UINT* pSizeOfData)