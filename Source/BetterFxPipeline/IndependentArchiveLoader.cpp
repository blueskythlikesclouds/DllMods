#include "IndependentArchiveLoader.h"

const std::array<const char*, 2> ARCHIVE_NAMES =
{
    "FxFXAA",
    "SWA"
};

FUNCTION_PTR(void*, __thiscall, fun69C270, 0x69C270, 
    void* This, const boost::shared_ptr<void>& a2, const SharedString& arFileName, const SharedString& arlFileName, void* a5);

FUNCTION_PTR(void*, __thiscall, fun69AFF0, 0x69AFF0, 
    void* This, boost::shared_ptr<void> a2, const SharedString& arlFileName);

FUNCTION_PTR(void*, __thiscall, fun69AB10, 0x69AB10,
    void* This, boost::shared_ptr<void> a3, const SharedString& arFileName, void* a6, uint32_t a7, uint32_t a8);

FUNCTION_PTR(void*, __thiscall, fun446F90, 0x446F90, void* This, uint32_t a2, uint32_t a3);
FUNCTION_PTR(void*, __thiscall, fun446E30, 0x446E30, void* This);

HOOK(void*, __stdcall, LoadApplicationAndShaders, 0xD6A580, void* This)
{
    // Just letting you know, I have no idea what the functions do. I just copied exactly what Generations does to inject the ARs.

    void* archiveDatabaseLoader = *(void**)(*(uint32_t*)((uint32_t)This + 4) + 200);

    for (auto& archiveName : ARCHIVE_NAMES)
    {
        SharedString arFileName((std::string(archiveName) + ".ar").c_str());
        SharedString arlFileName((std::string(archiveName) + ".arl").c_str());

        uint32_t unk0[53];

        fun446F90((void*)&unk0, 200, 5);
        fun69C270(archiveDatabaseLoader, nullptr, arFileName, arlFileName, (void*)&unk0);
        fun446E30((void*)&unk0);

        uint32_t field04 = *(uint32_t*)((uint32_t)This + 4);
        boost::shared_ptr<void> field88 = *(boost::shared_ptr<void>*)(field04 + 136);

        fun69AFF0(archiveDatabaseLoader, field88, arlFileName);

        fun446F90((void*)&unk0, -10, 5);
        fun69AB10(archiveDatabaseLoader, field88, arFileName, (void*)&unk0, 0, 0);
        fun446E30((void*)&unk0);
    }

    return originalLoadApplicationAndShaders(This);
}

bool IndependentArchiveLoader::enabled = false;

void IndependentArchiveLoader::applyPatches()
{
    if (enabled)
        return;

    enabled = true;

    INSTALL_HOOK(LoadApplicationAndShaders);
}
