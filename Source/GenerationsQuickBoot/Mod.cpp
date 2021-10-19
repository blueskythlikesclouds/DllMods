// #include "LostCodeLoader.h" 
#include <vector>
#include "shellapi.h"
#include "MainSequenceMod.h"

char* luaData;
size_t luaDataLength;

HOOK(void*, __cdecl, LoadLuanneData, 0x6D93E0, const char*& name, const uint8_t* data, const size_t length, void* a4)
{
    if (strcmp(name, "MainSequence") == 0)
        return originalLoadLuanneData(name, (const uint8_t*)luaData, luaDataLength, a4);

    return originalLoadLuanneData(name, data, length, a4);
}

uint64_t classicSkill = 0;
HOOK(int, __fastcall, ClassicSonicSetSkill, 0xDC06A0, void* context, void* Edx, uint64_t* skill)
{
    *skill = classicSkill;
    return originalClassicSonicSetSkill(context, Edx, skill);
}

uint64_t modernSkill = 0;
HOOK(int, __fastcall, ModernSonicSetSkill, 0xDFE980, void* context, void* Edx, uint64_t* skill)
{
    *skill = modernSkill;
    return originalModernSonicSetSkill(context, Edx, skill);
}

uint32_t GetMultiLevelAddress(uint32_t initAddress, std::vector<uint32_t> offsets)
{
    uint32_t address = *(uint32_t*)initAddress;
    for (uint32_t i = 0; i < offsets.size(); i++)
    {
        uint32_t const& offset = offsets[i];
        address += offset;

        if (i < offsets.size() - 1)
        {
            address = *(uint32_t*)address;
        }
    }
    return address;
}

enum LanguageType
{
    English = 0,
    Japanese,
    French,
    German,
    Spanish,
    Italian,
    COUNT
};

bool optionApplied = false;
LanguageType voiceOverLanguage = LanguageType::English;
LanguageType uiLanguage = LanguageType::English;
int musicVolume = 100;
int effectsVolume = 100;
bool controlTutorial = true;
bool omochaoHints = true;
bool subtitles = true;

extern "C" __declspec(dllexport) void Init()
{
    INIReader reader("GenerationsQuickBoot.ini");

    if (reader.ParseError() != 0)
        MessageBox(NULL, L"Failed to parse GenerationsQuickBoot.ini", NULL, MB_ICONERROR);

    int argc{};
    wchar_t** ppArgs = CommandLineToArgvW(GetCommandLineW(), &argc);

    int playerClass{ -1 };
    std::string levelName;
	
	for (int i = 0; i < argc; i++)
	{
        wchar_t* pArg = ppArgs[i];
		if (!wcscmp(pArg, L"--quickboot_level"))
		{
            int nextArg = i + 1;
            if (nextArg >= argc)
                break;

            std::wstring arg = ppArgs[nextArg];
            levelName = std::string(arg.begin(), arg.end());
		}
        if (!wcscmp(pArg, L"--quickboot_player"))
        {
            int nextArg = i + 1;
            if (nextArg >= argc)
                break;

            playerClass = wcstol(ppArgs[nextArg], nullptr, 0);
        }
	}

    LocalFree(ppArgs);
	
    luaData = (char*)malloc(1024 + MainSequenceModSize);

    const size_t declarationsLength = sprintf(luaData,
        "global(\"StageName\", \"%s\");\n"
        "global(\"StageMission\", %d);\n"
        "global(\"PlayerClass\", %d);\n"
        "global(\"ForcePlayerClassOnRestart\", %d);\n"
        "global(\"ForcePlayerClassOnGoalReach\", %d);\n"
        "global(\"g_IsDebugSequence\", 0);\n"
        "global(\"g_IsQuickBoot\", %d);\n"
        "global(\"ExitType\", %d);\n"
        "global(\"ExitTypeOnGoalReach\", %d);\n",

        levelName.empty() ? reader.Get("QuickBoot", "StageName", "ghz200").c_str() : levelName.c_str(),
        reader.GetInteger("QuickBoot", "StageMission", 0),
        playerClass == -1 ? reader.GetInteger("QuickBoot", "PlayerClass", 0) : playerClass,
        reader.GetBoolean("QuickBoot", "ForcePlayerClassOnRestart", false),
        reader.GetBoolean("QuickBoot", "ForcePlayerClassOnGoalReach", false),
        levelName.empty() ? reader.GetInteger("QuickBoot", "g_IsQuickBoot", 0) : 3,
        reader.GetInteger("QuickBoot", "ExitType", 0),
        reader.GetInteger("QuickBoot", "ExitTypeOnGoalReach", 0)
    );

    memcpy(luaData + declarationsLength, MainSequenceMod, MainSequenceModSize);
    luaDataLength = declarationsLength + MainSequenceModSize;

    INSTALL_HOOK(LoadLuanneData);

    std::atexit([]() { free(luaData); });

    // Read options
    voiceOverLanguage = (LanguageType)reader.GetInteger("Options", "VoiceOver", 0);
    if (voiceOverLanguage < 0 || voiceOverLanguage >= LanguageType::COUNT)
    {
        voiceOverLanguage = LanguageType::English;
    }

    uiLanguage = (LanguageType)reader.GetInteger("Options", "UI", 0);
    if (uiLanguage < 0 || uiLanguage >= LanguageType::COUNT)
    {
        uiLanguage = LanguageType::English;
    }

    musicVolume = reader.GetInteger("Options", "MusicVolume", 100);
    if (musicVolume < 0 || musicVolume > 100)
    {
        musicVolume = 100;
    }

    effectsVolume = reader.GetInteger("Options", "EffectsVolume", 100);
    if (effectsVolume < 0 || effectsVolume > 100)
    {
        effectsVolume = 100;
    }

    controlTutorial = reader.GetBoolean("Options", "ControlTutorial", true);
    omochaoHints = reader.GetBoolean("Options", "OmochaoHints", true);
    subtitles = reader.GetBoolean("Options", "Subtitles", true);

    // Force set skill
    bool setSkill = reader.GetBoolean("Skills", "ForceSetSkills", false);
    if (setSkill)
    {
        INSTALL_HOOK(ClassicSonicSetSkill);
        INSTALL_HOOK(ModernSonicSetSkill);

        bool powerBrake = reader.GetBoolean("Skills", "PowerBrake", false);
        bool sureFooted = reader.GetBoolean("Skills", "SureFooted", false);
        bool safetyNet = reader.GetBoolean("Skills", "SafetyNet", false);
        bool lastChance = reader.GetBoolean("Skills", "LastChance", false);
        bool tenSecondRings = reader.GetBoolean("Skills", "TenSecondRings", false);
        bool atheticism = reader.GetBoolean("Skills", "Athleticism", false);
        bool timeBrake = reader.GetBoolean("Skills", "TimeBrake", false);
        bool superSonic = reader.GetBoolean("Skills", "SuperSonic", false);

        classicSkill |= (powerBrake ? 0x10 : 0);
        classicSkill |= (sureFooted ? 0x20 : 0);
        classicSkill |= (safetyNet ? 0x40 : 0);
        classicSkill |= (lastChance ? 0x80 : 0);
        classicSkill |= (tenSecondRings ? 0x200 : 0);
        classicSkill |= (atheticism ? 0x400 : 0);
        classicSkill |= (timeBrake ? 0x800 : 0);
        classicSkill |= (superSonic ? 0x1000 : 0);
        classicSkill |= (reader.GetBoolean("Skills", "FlameShield", false) ? 0x2000 : 0);
        classicSkill |= (reader.GetBoolean("Skills", "AquaShield", false) ? 0x4000 : 0);
        classicSkill |= (reader.GetBoolean("Skills", "ThunderShield", false) ? 0x8000 : 0);
        classicSkill |= (reader.GetBoolean("Skills", "TwinSpinAttack", false) ? 0x10000 : 0);
        classicSkill |= (reader.GetBoolean("Skills", "InvincibleStart", false) ? 0x20000 : 0);
        classicSkill |= (reader.GetBoolean("Skills", "Shield", false) ? 0x40000 : 0);
        classicSkill |= (reader.GetBoolean("Skills", "PowerSneakers", false) ? 0x80000 : 0);
        classicSkill |= (reader.GetBoolean("Skills", "LongSpinDash", false) ? 0x100000 : 0);
        classicSkill |= (reader.GetBoolean("Skills", "TreasureScanner", false) ? 0x200000 : 0);
        classicSkill |= (reader.GetBoolean("Skills", "HomingAttack", false) ? 0x400000 : 0);
        classicSkill |= (reader.GetBoolean("Skills", "HerculesMuscles", false) ? 0x800000 : 0);
        classicSkill |= (reader.GetBoolean("Skills", "BreathHolder", false) ? 0x1000000 : 0);
        classicSkill |= (reader.GetBoolean("Skills", "Boardmaster", false) ? 0x2000000 : 0);
        classicSkill |= (reader.GetBoolean("Skills", "RingTime", false) ? 0x4000000 : 0);

        modernSkill |= (powerBrake ? 0x10 : 0);
        modernSkill |= (sureFooted ? 0x20 : 0);
        modernSkill |= (safetyNet ? 0x40 : 0);
        modernSkill |= (lastChance ? 0x80 : 0);
        modernSkill |= (tenSecondRings ? 0x200 : 0);
        modernSkill |= (atheticism ? 0x400 : 0);
        modernSkill |= (timeBrake ? 0x800 : 0);
        modernSkill |= (superSonic ? 0x1000 : 0);
        modernSkill |= (reader.GetBoolean("Skills", "BlastOff", false) ? 0x8000000 : 0);
        modernSkill |= (reader.GetBoolean("Skills", "SpeedUp", false) ? 0x10000000 : 0);
        modernSkill |= (reader.GetBoolean("Skills", "DirectJump", false) ? 0x20000000 : 0);
        modernSkill |= (reader.GetBoolean("Skills", "PowerStomp", false) ? 0x40000000 : 0);
        modernSkill |= (reader.GetBoolean("Skills", "QuickSlide", false) ? 0x80000000 : 0);
        modernSkill |= (reader.GetBoolean("Skills", "Acceleration", false) ? 0x100000000 : 0);
        modernSkill |= (reader.GetBoolean("Skills", "SmasherBonus", false) ? 0x200000000 : 0);
        modernSkill |= (reader.GetBoolean("Skills", "RingEnergyBonus", false) ? 0x400000000 : 0);
        modernSkill |= (reader.GetBoolean("Skills", "BoostGaugeUp", false) ? 0x800000000 : 0);
        modernSkill |= (reader.GetBoolean("Skills", "SerialHoming", false) ? 0x1000000000 : 0);
        modernSkill |= (reader.GetBoolean("Skills", "AutoGauge", false) ? 0x2000000000 : 0);
        modernSkill |= (reader.GetBoolean("Skills", "GaugeRebooster", false) ? 0x4000000000 : 0);
        modernSkill |= (reader.GetBoolean("Skills", "TrickMastery", false) ? 0x8000000000 : 0);
        modernSkill |= (reader.GetBoolean("Skills", "EndlessBoost", false) ? 0x10000000000 : 0);
    }
}

extern "C" void __declspec(dllexport) OnFrame()
{
    // Apply options after pointers are populated
    if (!optionApplied)
    {
        uint32_t voiceOverAddress = GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x7C, 0x10 });
        *(uint8_t*)voiceOverAddress = (uint8_t)voiceOverLanguage;

        uint32_t uiAddress = GetMultiLevelAddress(0x1E66B34, { 0x8 });
        *(uint8_t*)uiAddress = (uint8_t)uiLanguage;

        // music volume [0.0 - 0.63]
        uint32_t musicVolumeAddress = GetMultiLevelAddress(0x1E77290, { 0x38 });
        *(float*)musicVolumeAddress = (float)musicVolume * 0.01f * 0.63f;

        // effect volume [0.0 - 0.63]
        uint32_t effectVolumeAddress = GetMultiLevelAddress(0x1E77290, { 0x3C });
        *(float*)effectVolumeAddress = (float)effectsVolume * 0.01f * 0.63f;

        uint32_t flagsAddress = GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x7C, 0x18 });
        *(uint8_t*)flagsAddress = 0x24;
        if (controlTutorial)    *(uint8_t*)flagsAddress += 0x1;
        if (omochaoHints)       *(uint8_t*)flagsAddress += 0x2;
        if (subtitles)          *(uint8_t*)flagsAddress += 0x10;

        optionApplied = true;
    }
}
