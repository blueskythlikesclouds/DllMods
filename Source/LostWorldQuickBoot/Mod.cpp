#include "LostCodeLoader.h"

std::string stageId;

enum Type { TYPE_NORMAL, TYPE_TO_TITLE, TYPE_TO_E3_TITLE, TYPE_TO_MULTISELECT, TYPE_TO_BATTLE, TYPE_TO_WORLD_MAP, TYPE_TO_STAGE, TYPE_TO_MINIGAME, TYPE_TO_DEVMENU } type;

enum ExitType { EXIT_TYPE_NORMAL, EXIT_TYPE_RELOAD } exitType;

FUNCTION_PTR(void*, __cdecl, copyString, ASLR(0x968AA0), const char* destination, const char* source, int length);

FUNCTION_PTR(void*, __fastcall, setDRCRenderMode, ASLR(0x4E5F30), void* This, void* edx, int a2);
FUNCTION_PTR(void*, __fastcall, setTVRenderMode, ASLR(0x4E5F60), void* This, void* edx, int a2);

HOOK(void*, __fastcall, StartUpState, ASLR(0x910C30), void* gameSequence, void* edx, void* a2, int* status)
{
	void* result = originalStartUpState(gameSequence, edx, a2, status);
	if (*status == 1)
	{
		// Go to save init directly (skipping the logos)
		*(uint32_t*)((uint32_t)gameSequence + 80) = ASLR(0x910690);
	}
	return result;
}

HOOK(void*, __fastcall, SaveInitState, ASLR(0x910690), void* gameSequence, void* edx, void* a2, int* status)
{
	void* result = originalSaveInitState(gameSequence, edx, a2, status);
	if (*status == 1)
	{
		// Go to state depending on choice
		copyString((const char*)((uint32_t)gameSequence + 64), stageId.c_str(), 16);

		switch (type)
		{
		case TYPE_TO_E3_TITLE:
			*(uint32_t*)((uint32_t)gameSequence + 80) = ASLR(0x90EAC0);
			break;

		case TYPE_TO_MULTISELECT:
			*(uint32_t*)((uint32_t)gameSequence + 80) = ASLR(0x90FB40);
			break;

		case TYPE_TO_WORLD_MAP:
			*(uint32_t*)((uint32_t)gameSequence + 80) = ASLR(0x911F20);
			break;

		case TYPE_TO_STAGE:
			*(uint32_t*)((uint32_t)gameSequence + 80) = ASLR(0x911000);
			break;

		case TYPE_TO_BATTLE:
			*(uint32_t*)((uint32_t)gameSequence + 80) = ASLR(0x90F590);
			break;

		case TYPE_TO_MINIGAME:
			*(uint32_t*)((uint32_t)gameSequence + 80) = ASLR(0x9108D0);
			break;

		case TYPE_TO_DEVMENU:
			*(uint32_t*)((uint32_t)gameSequence + 80) = ASLR(0x9101A0);
			break;
		}
	}
	return result;
}

HOOK(void*, __fastcall, StageState, ASLR(0x911000), void* gameSequence, void* edx, void* a2, int* status)
{
	void* result = originalStageState(gameSequence, edx, a2, status);
	if (*status == 1 && (*(uint32_t*)((uint32_t)gameSequence + 80) == ASLR(0x910370) || *(uint32_t*)((uint32_t)
		gameSequence + 80) == ASLR(0x911F20)))
	{
		// Reload the stage on exit
		copyString((const char*)((uint32_t)gameSequence + 64), stageId.c_str(), 16);
		*(uint32_t*)((uint32_t)gameSequence + 80) = ASLR(0x911000);
	}
	return result;
}

int currentTV = 1;
int currentDRC = 1;

HOOK(LRESULT, __stdcall, MessageWindowProcessCallback, ASLR(0x401410), HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (Msg == WM_KEYDOWN && wParam == VK_F1)
		setTVRenderMode(*(void**)ASLR(0xFD3CC4), NULL, currentTV = currentTV == 1 ? 2 : 1);
	else if (Msg == WM_KEYDOWN && wParam == VK_F2)
		setDRCRenderMode(*(void**)ASLR(0xFD3CC4), NULL, currentDRC = currentDRC == 1 ? 2 : 1);

	return originalMessageWindowProcessCallback(hWnd, Msg, wParam, lParam);
}

extern "C" void __declspec(dllexport) __cdecl Init(ModInfo *modInfo)
{
	std::string dir = modInfo->CurrentMod->Path;

	size_t pos = dir.find_last_of("\\/");
	if (pos != std::string::npos)
		dir.erase(pos + 1);

	INIReader* reader = new INIReader("QuickBoot.ini");
	if (reader->ParseError() != 0)
		delete reader, reader = new INIReader(dir + "QuickBoot.ini");

	if (reader->ParseError() != 0)
		MessageBox(NULL, L"Failed to parse QuickBoot.ini", NULL, MB_ICONERROR);

	stageId = reader->Get("QuickBoot", "StageId", "w1a01");
	type = static_cast<Type>(reader->GetInteger("QuickBoot", "Type", TYPE_TO_STAGE));
	exitType = static_cast<ExitType>(reader->GetInteger("QuickBoot", "ExitType", EXIT_TYPE_NORMAL));

	std::transform(stageId.begin(), stageId.end(), stageId.begin(), ::tolower);

	if (type > TYPE_NORMAL)
		INSTALL_HOOK(StartUpState);

	if (type > TYPE_TO_TITLE)
		INSTALL_HOOK(SaveInitState);

	if (exitType == EXIT_TYPE_RELOAD)
		INSTALL_HOOK(StageState);

	INSTALL_HOOK(MessageWindowProcessCallback);

	WRITE_MEMORY(ASLR(0x939D0E), uint8_t, 0x01);

	delete reader;
}