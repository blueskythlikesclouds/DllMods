#pragma once

#include "Buffer.h"

class VertexBuffer : public Buffer
{
public:
    VertexBuffer(Device* device, size_t length, D3D11_BIND_FLAG bindFlags);

    virtual HRESULT GetDesc(D3DVERTEXBUFFER_DESC* pDesc) final;
};
