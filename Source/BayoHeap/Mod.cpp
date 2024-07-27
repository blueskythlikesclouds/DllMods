HOOK(BOOL, __fastcall, HeapPhysicalCreate, 0xC6E540, void* This, void* _, size_t size, void* heap)
{
	*(reinterpret_cast<void**>(This) + 9) = heap;
	return originalHeapPhysicalCreate(This, _, size, heap);
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

		WRITE_MEMORY(0x49E901, uint32_t, 0xFFFFFFFF); // global heap size
		INSTALL_HOOK(HeapPhysicalCreate);
	}

	return TRUE;
}
