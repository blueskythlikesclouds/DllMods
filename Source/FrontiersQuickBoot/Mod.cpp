static void fatalError(const TCHAR* message)
{
    MessageBox(nullptr, message, TEXT("Quick Boot"), MB_ICONERROR);
    exit(-1);
}

SIG_SCAN
(
    sigTinyFsmSignalLeave, 
    0x14FF42640,
    "\x0F\xB6\x41\x30\xA8\x01", 
    "xxxxxx"
)

FUNCTION_PTR(void, __fastcall, tinyFsmSignalLeave, sigTinyFsmSignalLeave(), size_t a1);

SIG_SCAN
(
    sigUINoticeBoardEnter,
    0x1409DC190,
    "\x48\x8B\xC4\x55\x53\x48\x8D\x68\xA1\x48\x81\xEC\x00\x00\x00\x00\x48\x89\x78\x18\x48\x8B\xD9\x4C\x89\x60\x20\x4C\x89\x68\xE8\x45\x33\xED\x4C\x89\x70\xE0\x44\x89\x6D\xDB\x4C\x89\x6D\xE7\x4C\x89\x78\xD8\xC6\x45\xD7\x00\xC6\x45\xEF\x01\xC7\x45\x00\x00\x00\x00\x00\xC7\x45\x00\x00\x00\x00\x00",
    "xxxxxxxxxxxx????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx?????xx?????"
)

HOOK(void, __fastcall, UINoticeBoardEnter, sigUINoticeBoardEnter(), size_t a1, size_t a2, size_t a3)
{
    tinyFsmSignalLeave(a1);
}

std::string stageId;
int stageType;

SIG_SCAN
(
    sigStateTitle,
    0x14015B4D0,
    "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x55\x57\x41\x56\x48\x8D\xAC\x24\xC0\xFE\xFF\xFF\x48\x81\xEC\x40\x02\x00\x00\x41",
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
)

SIG_SCAN
(
    sigStateFishing,
    0x140159440,
    "\x48\x89\x74\x24\x00\x57\x48\x81\xEC\x00\x00\x00\x00\x41\x8B\x00\x48\x8B\xF2\x48\x8B\xF9\x83\xF8\xFC\x0F\x84\x00\x00\x00\x00\x83\xF8\xFD\x0F\x84\x00\x00\x00\x00\x83\xF8\x07\x0F\x85\x00\x00\x00\x00\x49\x8B\x40\x08\x81\x78\x00\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00\x8B\x40\x20\x3D\x00\x00\x00\x00\x74\x7D",
    "xxxx?xxxx????xxxxxxxxxxxxxx????xxxxx????xxxxx????xxxxxx?????xx????xxxx????xx"
)

SIG_SCAN
(
    sigStateHacking,
    0x140159CC0,
    "\x48\x89\x5C\x24\x00\x55\x57\x41\x56\x48\x8D\x6C\x24\x00\x48\x81\xEC\x00\x00\x00\x00\x41\x8B\x00\x4C\x8B\xF2",
    "xxxx?xxxxxxxx?xxx????xxxxxx"
)

SIG_SCAN
(
    sigStateStage,
    0x14015A9C0,
    "\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x4C\x89\x74\x24\x00\x55\x48\x8D\xAC\x24\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x41\x8B\x00",
    "xxxx?xxxx?xxxx?xxxx?xxxxx????xxx????xxx"
)

SIG_SCAN
(
    sigStateTutorial,
    0x14015BFB0,
    "\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x4C\x89\x74\x24\x00\x55\x48\x8D\x6C\x24\x00\x48\x81\xEC\x00\x00\x00\x00\x41\x8B\x00\x48\x8B\xF2",
    "xxxx?xxxx?xxxx?xxxx?xxxxx?xxx????xxxxxx"
)

SIG_SCAN
(
    sigStateCyberStage,
    0x1401588C0,
    "\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x41\x8B\x00",
    "xxxx?xxxx?xxxx?xxxxxxxxx????xxx????xxx"
)

SIG_SCAN
(
    sigTinyFsmDelegate,
    0x140124F20,
    "\x40\x53\x48\x83\xEC\x40\x49\x8B\x00\x48\x8B\xD9\x49\x63\x48\x08\x4D\x8B\xC1\x48\x03\xCA\x48\x8D\x54\x24\x00\xFF\xD0\x48\x8B\xD0\x48\x8B\xCB\xE8\x00\x00\x00\x00\x48\x8B\xC3\x48\x83\xC4\x40\x5B\xC3\xCC\xCC\xCC\x48\x83\xE9\x70",
    "xxxxxxxxxxxxxxxxxxxxxxxxxx?xxxxxxxxx????xxxxxxxxxxxxxxxx"
)

SIG_SCAN
(
    sigAutoSave,
    0x14D03C9A2,
    "\x74\x0D\x30\xC0\x48\x8B\x5C\x24\x00\x48\x83\xC4\x20\x5E\xC3",
    "xxxxxxxx?xxxxxx"
)

HOOK(size_t, __fastcall, StateTitle, sigStateTitle(), size_t a1, size_t a2, size_t a3)
{
    int signal = *(int*)a3;

    static bool neverMore;
    if (neverMore)
    {
        if (signal == -3)
            WRITE_MEMORY(sigAutoSave(), uint8_t, 0x74, 0x0D);

        return originalStateTitle(a1, a2, a3);
    }

    unsigned char type = 2;
    void* function = nullptr;

    switch (signal)
    {
    case 0:
        switch (stageType)
        {
        case 'f': 
            function = sigStateFishing();
            break;

        case 'h': 
            function = sigStateHacking();
            break;

        case 'r': 
            function = sigStateStage();
            *(unsigned short*)(a1 + 312) |= 4;
            break;

        case 't':
            function = sigStateTutorial();
            break;

        case 'd':
            function = sigStateCyberStage();
            *(unsigned char*)(a1 + 290) |= 1;
            break;

        default:
            fatalError(TEXT("Cannot determine stage type"));
        }

        *(void**)(a1 + 128) = function;
        *(size_t*)(a1 + 136) = 0;
        *(void**)(a1 + 144) = sigTinyFsmDelegate();
        *(size_t*)(a1 + 152) = 1;
        strncpy((char*)(a1 + 160), stageId.c_str(), 16);

    case -4:
        neverMore = true;

    case -3:
        type = 3;
        break;
    }

    *(size_t*)(a2 + 16) = 0;
    *(unsigned char*)(a2 + 24) = type;
    *(size_t*)a2 = 0;
    *(size_t*)(a2 + 8) = 0;

    return a2;
}

SIG_SCAN
(
    sigSaveSlot,
    0x1401BAA6C,
    "\x66\xC7\x45\x20\x00\x00", 
    "xxxxxx"
)

SIG_SCAN
(
    sigTelemetry1,
    0x14007E7D0,
    "\x48\x83\xEC\x48\x80\x3D", 
    "xxxxxx"
)

SIG_SCAN
(
    sigTelemetry2,
    0x14007E890,
    "\x40\x55\x53\x56\x57\x48\x8D\xAC\x24\x98", 
    "xxxxxxxxxx"
)

SIG_SCAN
(
    sigTelemetry3,
    0x140083F99,
    "\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\xD8\x89\x7C\x24\x28", 
    "x????x????xxxxxxx"
)

extern "C" __declspec(dllexport) void Init()
{
#if _DEBUG
    MessageBox(nullptr, TEXT("Attach to Process"), TEXT("Quick Boot"), MB_OK);
#endif

    if (!sigValid)
        fatalError(TEXT("Unsupported game version"));

    const INIReader reader("FrontiersQuickBoot.ini");
    if (reader.ParseError() != 0)
        fatalError(TEXT("Cannot load configuration file"));

    enum
    {
        NORMAL,
        TITLE,
        STAGE
    };

    stageId = reader.Get("Mod", "StageId", std::string());
    int bootType = reader.GetInteger("Mod", "Type", NORMAL);
    int saveSlot = reader.GetInteger("Mod", "SaveSlot", 3);
    bool disableAutoSave = reader.GetBoolean("Mod", "DisableAutoSave", true);

    if (stageId.size() < 3)
        fatalError(TEXT("Invalid stage id"));

    stageType = stageId[2];

    if (stageId == "w5r01")
        stageType = 'h';

    if (bootType != NORMAL)
    {
        INSTALL_HOOK(UINoticeBoardEnter);

        if (bootType != TITLE)
        {
            INSTALL_HOOK(StateTitle);
            WRITE_MEMORY((size_t)sigSaveSlot() + 4, uint8_t, (uint8_t)saveSlot);

            if (disableAutoSave)
                WRITE_NOP(sigAutoSave(), 2);
        }
    }

    // boot time increasing telemetry begone!
    WRITE_MEMORY(sigTelemetry1(), uint8_t, 0xC3);
    WRITE_MEMORY(sigTelemetry2(), uint8_t, 0xC3);
    WRITE_NOP(sigTelemetry3(), 5);
}