const char* const volatile MAIN_SEQUENCE_MOD = "dofile(\"MainSequenceMod.lua\");";

HOOK(void*, __cdecl, LoadLuanneData, 0x6D93E0, const char*& name, const uint8_t* data, const size_t length, void* a4)
{
    if (strcmp(name, "MainSequence") == 0)
        return originalLoadLuanneData(name, (const uint8_t*)MAIN_SEQUENCE_MOD, strlen(MAIN_SEQUENCE_MOD), a4);

    return originalLoadLuanneData(name, data, length, a4);
}

extern "C" __declspec(dllexport) void Init()
{
    INSTALL_HOOK(LoadLuanneData);
}