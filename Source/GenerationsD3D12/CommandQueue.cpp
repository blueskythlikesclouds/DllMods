#include "CommandQueue.h"

CommandQueue::~CommandQueue()
{
    CloseHandle(d3dFenceEvent);
}

ID3D12CommandQueue* CommandQueue::getD3DCommandQueue() const
{
    return d3dCommandQueue.Get();
}

ID3D12GraphicsCommandList* CommandQueue::getD3DCommandList() const
{
    return d3dCommandList.Get();
}

std::lock_guard<CriticalSection> CommandQueue::lock()
{
    return std::lock_guard(criticalSection);
}

void CommandQueue::initialize(const ComPtr<ID3D12Device>& d3dDevice, D3D12_COMMAND_LIST_TYPE type)
{
    // Create command queue description.
    D3D12_COMMAND_QUEUE_DESC desc;
    desc.Type = type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    // Create command queue.
    d3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3dCommandQueue));

    // Create command list.
    d3dDevice->CreateCommandAllocator(type, IID_PPV_ARGS(&d3dCommandAllocator));
    d3dDevice->CreateCommandList(
        0,
        type,
        d3dCommandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&d3dCommandList));

    // Create fence.
    d3dDevice->CreateFence(
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&d3dFence));

    d3dFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    d3dFenceValue = 1;
}

void CommandQueue::waitForFenceEvent()
{
    d3dCommandQueue->Signal(d3dFence.Get(), d3dFenceValue);
    d3dFence->SetEventOnCompletion(d3dFenceValue, d3dFenceEvent);
    WaitForSingleObjectEx(d3dFenceEvent, INFINITE, FALSE);
    ++d3dFenceValue;
}

void CommandQueue::resetCommandList() const
{
    d3dCommandAllocator->Reset();
    d3dCommandList->Reset(d3dCommandAllocator.Get(), nullptr);
}

void CommandQueue::submitAll()
{
    executeCommandList();
    waitForFenceEvent();
    resetCommandList();
}

void CommandQueue::executeCommandList() const
{
    d3dCommandList->Close();

    ID3D12CommandList* commandLists[] = { d3dCommandList.Get() };
    d3dCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}
