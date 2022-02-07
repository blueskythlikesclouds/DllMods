#include "MemoryHandler.h"

constexpr const char* ALLOC_SIGNATURE = " D3D9Ex ";

HANDLE hHeap = GetProcessHeap();

void* operator new(const size_t byteSize)
{
    return HeapAlloc(hHeap, 0, byteSize);
}

void operator delete(void* pMem)
{
    HeapFree(hHeap, 0, pMem);
}

HOOK(HANDLE*, __cdecl, GetHeapHandle, 0x660CF0, HANDLE& hHandle, size_t index)
{
    hHandle = hHeap;
    return &hHandle;
}

void* __cdecl alloc(const size_t byteSize)
{
    void* pMem = HeapAlloc(hHeap, 0, byteSize + 8);
    if (((uintptr_t)pMem % 16) == 0)
    {
        void* pMemRealloc = HeapReAlloc(hHeap, HEAP_REALLOC_IN_PLACE_ONLY, pMem, byteSize);
        return pMemRealloc ? pMemRealloc : pMem;
    }

    memcpy(pMem, ALLOC_SIGNATURE, 8);
    return &((uintptr_t*)pMem)[2];
}

void __cdecl dealloc(void* pMem)
{
    if (!pMem)
        return;

    const uintptr_t* pSignature = &((uintptr_t*)pMem)[-2];
    HeapFree(hHeap, 0, (LPVOID)(memcmp(pSignature, ALLOC_SIGNATURE, 8) == 0 ? pSignature : pMem));
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
