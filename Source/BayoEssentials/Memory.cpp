#include "Memory.h"

struct Heap
{
	INSERT_PADDING(0x24);
	Heap* subHeap;
};

HOOK(BOOL, __fastcall, HeapPhysicalCreate, 0xC6E540, Heap* This, void* _, size_t size, Heap* heap)
{
	This->subHeap = reinterpret_cast<Heap*>(0x5A57944);
	return originalHeapPhysicalCreate(This, _, size, heap);
}

void Memory::init()
{
    WRITE_MEMORY(0x49E901, uint32_t, 0xFFFFFFFF); // global heap size
    INSTALL_HOOK(HeapPhysicalCreate);
}