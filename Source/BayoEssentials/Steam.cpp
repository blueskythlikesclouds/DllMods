#include "Steam.h"

HOOK(bool, __cdecl, SteamAPI_RestartAppIfNecessary, PROC_ADDRESS("steam_api.dll", "SteamAPI_RestartAppIfNecessary"), uint32_t appId)
{
    originalSteamAPI_RestartAppIfNecessary(appId);
    FILE* file = fopen("steam_appid.txt", "w");
    fprintf(file, "%d", appId);
    fclose(file);
    return false;
}

void Steam::init()
{
    INSTALL_HOOK(SteamAPI_RestartAppIfNecessary);
}