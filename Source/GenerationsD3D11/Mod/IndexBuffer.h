#pragma once
#include "Buffer.h"

class IndexBuffer : public Buffer 
{
public:
    IndexBuffer(Device* device, size_t length, D3D11_BIND_FLAG bindFlags, DXGI_FORMAT format);

    virtual HRESULT GetDesc(D3DINDEXBUFFER_DESC* pDesc) final;
};
