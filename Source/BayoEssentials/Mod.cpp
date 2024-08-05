#include "Memory.h"
#include "QuickBoot.h"
#include "Fmerge.h"
#include "Awb.h"

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
#ifdef _DEBUG
        if (!GetConsoleWindow())
            AllocConsole();

        freopen("CONOUT$", "w", stdout);
#endif
        const INIReader reader("BayoEssentials.ini");

        Memory::init();
        QuickBoot::init(reader);
        Fmerge::init();
        Awb::init();
    }

    return TRUE;
}
