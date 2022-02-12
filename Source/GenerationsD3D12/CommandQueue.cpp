#include "CommandQueue.h"

CommandQueue::~CommandQueue()
{
    CloseHandle(fenceEvent);
}

ID3D12CommandQueue* CommandQueue::getCommandQueue() const
{
    return commandQueue.Get();
}

ID3D12GraphicsCommandList* CommandQueue::getCommandList() const
{
    return commandList.Get();
}

std::lock_guard<CriticalSection> CommandQueue::lock()
{
    return std::lock_guard(criticalSection);
}

void CommandQueue::initialize(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
{
    // Create command queue description.
    D3D12_COMMAND_QUEUE_DESC desc;
    desc.Type = type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    // Create command queue.
    device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue));

    // Create command list.
    device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator));
    device->CreateCommandList(
        0,
        type,
        commandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&commandList));

    // Create fence.
    device->CreateFence(
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&fence));

    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    fenceValue = 1;
}

void CommandQueue::waitForFenceEvent()
{
    commandQueue->Signal(fence.Get(), fenceValue);
    fence->SetEventOnCompletion(fenceValue, fenceEvent);
    WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
    ++fenceValue;
}

void CommandQueue::resetCommandList() const
{
    commandAllocator->Reset();
    commandList->Reset(commandAllocator.Get(), nullptr);
}

void CommandQueue::submitAll()
{
    executeCommandList();
    waitForFenceEvent();
    resetCommandList();
}

void CommandQueue::executeCommandList() const
{
    commandList->Close();

    ID3D12CommandList* commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}
