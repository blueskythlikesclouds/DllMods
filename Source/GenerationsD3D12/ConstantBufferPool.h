#pragma once

#include "ConstantBuffer.h"

class ConstantBufferPool
{
    size_t constantBufferSize;
    std::list<ConstantBuffer> buffers;
    std::list<ConstantBuffer>::const_iterator current;

public:
    ConstantBufferPool(size_t constantBufferSize);

    const ConstantBuffer& allocate(ID3D12Device* device, D3D12MA::Allocator* allocator);
    void reset();
};
