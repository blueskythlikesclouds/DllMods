std::string stageId;

enum Type { TYPE_NORMAL, TYPE_TO_TITLE, TYPE_TO_STAGE } type;

enum ExitType { EXIT_TYPE_NORMAL, EXIT_TYPE_RELOAD } exitType;

FUNCTION_PTR(void*, __fastcall, copyString, 0x1408688A0, const char* destination, const char* source, unsigned int size);

HOOK(void*, __fastcall, SaveInitState, 0x140149A20, void* gameSequence, void* a2, int* status)
{
	void* result = originalSaveInitState(gameSequence, a2, status);

	if (*status == 1)
	{
		*(uint64_t*)((uint64_t)gameSequence + 136) = 0x14014A3B0;
		*(uint64_t*)((uint64_t)gameSequence + 152) = 0x140106280;
		copyString((char*)((uint64_t)gameSequence + 200), stageId.c_str(), 0x40);
	}

	return result;
}


HOOK(void*, __fastcall, StageState, 0x14014A3B0, void* gameSequence, void* a2, int* status)
{
	void* result = originalStageState(gameSequence, a2, status);

	if (*status == 1)
	{
		*(uint64_t*)((uint64_t)gameSequence + 136) = (uint64_t)implOfSaveInitState;
		*(uint64_t*)((uint64_t)gameSequence + 152) = 0x140106280;
	}
	return result;
}

extern "C" __declspec(dllexport) void Init(const char* path)
{
	INIReader* reader = new INIReader("QuickBoot.ini");
	if (reader->ParseError() != 0)
		delete reader, reader = new INIReader(std::string(path) + "\\QuickBoot.ini");

	if (reader->ParseError() != 0)
		MessageBox(NULL, L"Failed to parse QuickBoot.ini", NULL, MB_ICONERROR);

	stageId = reader->Get("QuickBoot", "StageId", "w1a01");
	type = (Type)reader->GetInteger("QuickBoot", "Type", TYPE_TO_STAGE);
	exitType = (ExitType)reader->GetInteger("QuickBoot", "ExitType", EXIT_TYPE_NORMAL);

	if (type != TYPE_NORMAL)
	{
		// Skip SEGA logo and go to Sonic Team logo directly
		WRITE_MEMORY(0x14058A73D, uint8_t, 0x5F, 0x08);

		// Don't add ANM_in
		WRITE_MEMORY(0x14058B050, uint8_t, 0x90, 0x90, 0x90, 0x90, 0x90);

		// Don't add ANM_loop
		WRITE_MEMORY(0x14058B066, uint8_t, 0x90, 0x90, 0x90, 0x90, 0x90);

		// Don't add ANM_out
		WRITE_MEMORY(0x14058B07C, uint8_t, 0x90, 0x90, 0x90, 0x90, 0x90);

		// Skip the first auto save text and go to the second one directly
		WRITE_MEMORY(0x1445D646C, uint8_t, 0xD0, 0x7D);

		// Don't add ANM_out
		WRITE_MEMORY(0x14059E2F8, uint8_t, 0x90, 0x90, 0x90, 0x90, 0x90);

		// Don't check whether or not animation finished
		WRITE_MEMORY(0x14059E273, uint8_t, 0x90, 0x90);
	}

	if (type == TYPE_TO_STAGE)
		INSTALL_HOOK(SaveInitState);

	if (exitType == EXIT_TYPE_RELOAD)
		INSTALL_HOOK(StageState);

    // Skip telemetry (garry.sgaas.net)
	WRITE_NOP(0x1406E01CB, 5);

	delete reader;
}