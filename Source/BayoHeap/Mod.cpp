static FUNCTION_PTR(BOOL, __thiscall, heapGlobalCreate, 0xC6EAF0, void* This, size_t size);

BOOL __fastcall heapPhysicalCreate(void* This, void* _, size_t size, void* heap)
{
	return heapGlobalCreate(This, 0xFFFFFFFF);
}

static FUNCTION_PTR(BOOL, __thiscall, heapGlobalDtor, 0xC6EC60, void* This, size_t flags);

void __fastcall heapPhysicalDtor(void* This)
{
	heapGlobalDtor(This, 0);
}

size_t __fastcall heapPhysicalGetAllocatableSize(void* This)
{
	return 0xFFFFFFFF;
}

// for create func
static void* s_vftable[16];

static HANDLE __stdcall getProcessHeap()
{
	return GetProcessHeap();
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
#ifdef _DEBUG
		if (!GetConsoleWindow())
			AllocConsole();

		freopen("CONOUT$", "w", stdout);
#endif

		memcpy(s_vftable, reinterpret_cast<void*>(0xEAC7C4), sizeof(s_vftable));
		s_vftable[15] = heapPhysicalCreate;

		WRITE_MEMORY(0x49E901, uint32_t, 0xFFFFFFFF); // global heap size
		WRITE_JUMP(0xC6E540, heapPhysicalCreate);
		WRITE_JUMP(0xC6E520, 0xC6EAD0); // heap physical ctor
		WRITE_MEMORY(0xC6EADA, void*, s_vftable); // heap global vtable
		WRITE_JUMP(0xC4C750, heapPhysicalDtor);
		WRITE_JUMP(0xC6E7B0, 0xC6EC30); // heap physical destroy
		WRITE_JUMP(0xC6E9B0, heapPhysicalGetAllocatableSize);

		// override HeapCreate
		WRITE_NOP(0xC6EB10, 0x7);
		WRITE_CALL(0xC6EB17, getProcessHeap);	
		WRITE_NOP(0xC6EB78, 0x7);
		WRITE_CALL(0xC6EB7F, getProcessHeap);
		WRITE_NOP(0xC6F131, 0x6);
		WRITE_NOP(0xC6F139, 0x1);
		WRITE_CALL(0xC6F13A, getProcessHeap);

		// nop HeapDestroy
		WRITE_NOP(0xC6EC10, 0x7);
		WRITE_NOP(0xC6EBAA, 0x7);
		WRITE_NOP(0xC6EC3A, 0x7);
		WRITE_NOP(0xC6EC70, 0x7);
		WRITE_NOP(0xC6EE0F, 0x7);

		// remove HEAP_NO_SERIALIZE from HeapAlloc
		WRITE_MEMORY(0xC6ECF9, uint8_t, 0);
		WRITE_MEMORY(0xC6ED28, uint8_t, 0);
		WRITE_MEMORY(0xC6ED58, uint8_t, 0);
		WRITE_MEMORY(0xC6EF48, uint8_t, 0);

		// remove HEAP_NO_SERIALIZE from HeapFree
		WRITE_MEMORY(0xC6ED99, uint8_t, 0);
		WRITE_MEMORY(0xC6EDB9, uint8_t, 0);
		WRITE_MEMORY(0xC6F0C9, uint8_t, 0);
	}

	return TRUE;
}
