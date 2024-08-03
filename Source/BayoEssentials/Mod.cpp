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

static uint32_t s_r000MidAsmHookRetAddr = 0x5A1D3A;
static bool s_handledR000MidAsmHook = false;

static void __declspec(naked) r000MidAsmHook()
{
	__asm
	{
		xor edi, edi
		mov [esi + 0x14], edi
		cmp s_handledR000MidAsmHook, 1
		jnz cond
		jmp[s_r000MidAsmHookRetAddr]
	cond:
		mov s_handledR000MidAsmHook, 1

		mov [esi + 0x24], 2
		mov [esi + 0x10], 4
		mov [esi + 0xC], edi
		mov [esi + 0x8], 4
		pop edi
		pop ebx
		pop esi
		retn
	}
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

		// file size removal
		WRITE_MEMORY(0x49E901, uint32_t, 0xFFFFFFFF); // global heap size
		INSTALL_HOOK(HeapPhysicalCreate);

		bool skipLogo = true;
		bool autoSaveQuickBoot = false;

		const INIReader reader("BayoEssentials.ini");
		if (reader.ParseError() == 0)
		{
			skipLogo = reader.GetBoolean("Main", "SkipLogo", skipLogo);
			autoSaveQuickBoot = reader.GetBoolean("Main", "AutoSaveQuickBoot", autoSaveQuickBoot);
		}

		if (skipLogo || autoSaveQuickBoot)
		{
			// logo + movie skip
			WRITE_MEMORY(0x459CE5, uint8_t, 0x83, 0xC4, 0x10, 0x90, 0x90);
			WRITE_MEMORY(0x459D07, uint8_t, 0xEB);
			WRITE_MEMORY(0x5A1C40, uint8_t, 0xC);
			WRITE_MEMORY(0x5A1C43, uint8_t, 0x8, 0x1);

			if (autoSaveQuickBoot)
				WRITE_JUMP(0x5A1D35, r000MidAsmHook);
		}
	}

	return TRUE;
}
