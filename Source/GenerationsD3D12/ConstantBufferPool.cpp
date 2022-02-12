#include "ConstantBufferPool.h"

ConstantBufferPool::ConstantBufferPool(const size_t constantBufferSize) : constantBufferSize(constantBufferSize), current(buffers.begin())
{
}

const ConstantBuffer& ConstantBufferPool::allocate(ID3D12Device* device, D3D12MA::Allocator* allocator)
{
    return (current == buffers.end() || ++current == buffers.end()) ? buffers.emplace_back(device, allocator, constantBufferSize) : *current;
}

void ConstantBufferPool::reset()
{
    current = buffers.begin();
}
