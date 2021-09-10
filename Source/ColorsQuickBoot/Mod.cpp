#include "SigScan.h"

enum class Type
{
    Normal,
    Title,
    WorldMap,
    Stage
} type;

int zoneIndex;
int actIndex;

HOOK(void*, __fastcall, StateStartUp, sigStateStartUp(), void* gameSequence, void* A2, int* status)
{
    void* result = originalStateStartUp(gameSequence, A2, status);

    if (*status == 1)
    {
        switch (type)
        {
        case Type::Title:
            *(void**)((char*)gameSequence + 88) = sigStateTitle();
            break;

        case Type::WorldMap:
            *(void**)((char*)gameSequence + 88) = sigStateWorldMap();
            break;

        case Type::Stage:
            *(int*)((char*)gameSequence + 120) = zoneIndex;
            *(int*)((char*)gameSequence + 124) = actIndex;
            *(void**)((char*)gameSequence + 88) = sigStateStage();
            break;

        default: break;
        }
    }

    return result;
}

extern "C" __declspec(dllexport) void Init()
{
    INSTALL_HOOK(StateStartUp);

    const INIReader reader("ColorsQuickBoot.ini");
    if (reader.ParseError() != 0)
        MessageBox(nullptr, TEXT("Failed to parse ColorsQuickBoot.ini"), TEXT("Quick Boot"), MB_ICONERROR);

    type = (Type)reader.GetInteger("QuickBoot", "Type", (int)Type::Normal);
    zoneIndex = reader.GetInteger("QuickBoot", "ZoneIndex", 0);
    actIndex = reader.GetInteger("QuickBoot", "ActIndex", 0);
}