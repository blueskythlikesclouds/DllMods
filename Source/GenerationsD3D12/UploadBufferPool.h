#pragma once

struct UploadBuffer
{
    D3D12MA::Allocation* allocation;
    void* data;
    size_t size;
    D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress;
};

class UploadBufferPool
{
    std::vector<std::pair<ComPtr<D3D12MA::Allocation>, void*>> buffers;
    size_t current{};
public:
    ~UploadBufferPool();

    UploadBuffer allocate(D3D12MA::Allocator* allocator, size_t size);
    void reset();
};
