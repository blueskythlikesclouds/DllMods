#pragma once

#include "CriticalSection.h"

class Device;

class CommandQueue
{
    ComPtr<ID3D12CommandQueue> commandQueue;
    ComPtr<ID3D12GraphicsCommandList> commandList;
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    ComPtr<ID3D12Fence> fence;
    HANDLE fenceEvent{};
    UINT64 fenceValue{};
    CriticalSection criticalSection;

public:
    explicit CommandQueue() = default;
    ~CommandQueue();

    ID3D12CommandQueue* getCommandQueue() const;
    ID3D12GraphicsCommandList* getCommandList() const;

    std::lock_guard<CriticalSection> lock();

    void initialize(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);

    void waitForFenceEvent();
    void executeCommandList() const;
    void resetCommandList() const;
    void submitAll();
};
