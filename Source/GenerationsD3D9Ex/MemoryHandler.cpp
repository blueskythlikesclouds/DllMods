#include "MemoryHandler.h"

#ifndef MEMORY_SIGNATURE
#define MEMORY_SIGNATURE " D3D9Ex "
#endif

static_assert(sizeof(MEMORY_SIGNATURE) == (8 + 1));

static HANDLE processHeap = GetProcessHeap();

HOOK(HANDLE*, __cdecl, GetHeapHandle, 0x660CF0, HANDLE& handle, size_t index)
{
    handle = processHeap;
    return &handle;
}

static void* __cdecl alloc(size_t byteSize)
{
    void* memory = HeapAlloc(processHeap, 0, byteSize + 8);

    if ((reinterpret_cast<uintptr_t>(memory) & 0xF) == 0)
    {
        HeapReAlloc(processHeap, HEAP_REALLOC_IN_PLACE_ONLY, memory, byteSize);
        return memory;
    }

    memcpy(memory, MEMORY_SIGNATURE, 8);
    return static_cast<uint8_t*>(memory) + 8;
}

static void __cdecl dealloc(void* memory)
{
    if (!memory)
        return;

    uint8_t* signature = static_cast<uint8_t*>(memory) - 8;

    if (memcmp(signature, MEMORY_SIGNATURE, 8) == 0)
    {
        memset(signature, 0, 8);
        HeapFree(processHeap, 0, signature);
    }
    else
    {
        HeapFree(processHeap, 0, memory);
    }
}

void MemoryHandler::applyPatches()
{
    // Don't init the memory allocator.
    WRITE_MEMORY(0x660C00, uint8_t, 0xC3);

    // Get a null handle.
    INSTALL_HOOK(GetHeapHandle);

    // Replace all functions.
    WRITE_JUMP(0x65FC60, alloc);
    WRITE_JUMP(0x65FC80, alloc);
    WRITE_JUMP(0x65FCA0, alloc);
    WRITE_JUMP(0x65FCC0, alloc);
    WRITE_JUMP(0x65FCE0, alloc);
    WRITE_JUMP(0x65FD00, alloc);
    WRITE_JUMP(0x65FD20, alloc);
    WRITE_JUMP(0x65FD40, alloc);
    WRITE_JUMP(0x65FD70, dealloc);
    WRITE_JUMP(0x65FD90, alloc);
    WRITE_JUMP(0x65FDB5, dealloc);
}
