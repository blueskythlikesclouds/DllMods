#pragma once

#include "CriticalSection.h"

class Device;

class CommandQueue
{
    ComPtr<ID3D12CommandQueue> d3dCommandQueue;
    ComPtr<ID3D12GraphicsCommandList> d3dCommandList;
    ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
    ComPtr<ID3D12Fence> d3dFence;
    HANDLE d3dFenceEvent{};
    UINT64 d3dFenceValue{};
    CriticalSection criticalSection;

public:
    explicit CommandQueue() = default;
    ~CommandQueue();

    ID3D12CommandQueue* getD3DCommandQueue() const;
    ID3D12GraphicsCommandList* getD3DCommandList() const;

    std::lock_guard<CriticalSection> lock();

    void initialize(const ComPtr<ID3D12Device>& d3dDevice, D3D12_COMMAND_LIST_TYPE type);

    void waitForFenceEvent();
    void executeCommandList() const;
    void resetCommandList() const;
    void submitAll();
};
