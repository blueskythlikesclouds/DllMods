#include "Memory.h"
#include "QuickBoot.h"
#include "Fmerge.h"
#include "Awb.h"
#include "Export.h"
#include "Steam.h"
#include "MatrixLimit.h"

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
#ifdef _DEBUG
        if (!GetConsoleWindow())
            AllocConsole();

        freopen("CONOUT$", "w", stdout);
#endif

        toml::table config;
        std::ifstream stream("config.toml");
        if (stream.is_open())
        {
            try
            {
                config = toml::parse(stream);
            }

            catch (std::exception& exception)
            {
                char text[0x400];
                sprintf_s(text, "Failed to parse config.toml:\n%s", exception.what());
                MessageBoxA(nullptr, text, "Bayonetta Essentials", MB_OK | MB_ICONERROR);
            }
        }

        Export::init();
        Memory::init();
        QuickBoot::init(config);
        Fmerge::init();
        Awb::init();
        Steam::init();
        MatrixLimit::init();
    }

    return TRUE;
}
