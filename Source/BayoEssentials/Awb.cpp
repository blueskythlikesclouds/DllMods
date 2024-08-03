#include "Awb.h"

struct CriAtomAwbTag
{
    INSERT_PADDING(0x8);
    char* path;
};

static FUNCTION_PTR(void*, __cdecl, criAtomAwbTocGetBinderHandle, 0xCB5460, CriAtomAwbTag* awb, uint32_t* id);
static FUNCTION_PTR(void, __cdecl, criAtomAwbGetWaveFileInfo, 0xCB5473, CriAtomAwbTag* awb, int32_t id, uint64_t* offset, uint32_t* size);

struct CriAtomPlayerTag
{
    INSERT_PADDING(0x124);
    uint32_t offset;
    uint32_t size;
};

static void criAtomPlayerSetFileCore(CriAtomPlayerTag* player, void* binder, const char* path, uint32_t offset_, uint32_t size_)
{
    static uint32_t funcAddr = 0xCA8C26;
    __asm
    {
        push size_
        push offset_
        push path
        push binder
        mov eax, player
        call[funcAddr]
        add esp, 0x10
    }
}

struct AdxFile
{
    int32_t id;
    uint32_t size;
    char path[MAX_PATH];
};

static std::vector<AdxFile> s_adxFilesBgm000;
static std::vector<AdxFile> s_adxFilesBgm001;

static BOOL __stdcall criAtomPlayerSetWaveIdFileHook(CriAtomPlayerTag* player, CriAtomAwbTag* awb, int32_t id)
{
    void* binder = criAtomAwbTocGetBinderHandle(awb, nullptr);

    auto adxFiles = &s_adxFilesBgm000;
    if (strcmp(awb->path, "bgm\\BGM001.awb") == 0)
        adxFiles = &s_adxFilesBgm001;

    for (auto& adxFile : *adxFiles)
    {
        if (adxFile.id == id)
        {
            criAtomPlayerSetFileCore(player, binder, adxFile.path, 0, adxFile.size);
            player->offset = 0;
            player->size = adxFile.size;
            return true;
        }
    }

    uint64_t offset;
    uint32_t size;
    criAtomAwbGetWaveFileInfo(awb, id, &offset, &size);
    criAtomPlayerSetFileCore(player, binder, awb->path, static_cast<uint32_t>(offset), size);
    player->offset = static_cast<uint32_t>(offset);
    player->size = size;

    return true;
}

static void __declspec(naked) criAtomPlayerSetWaveIdFileTrampoline()
{
    __asm
    {
        push [ebp + 8]
        push esi
        push edi
        call criAtomPlayerSetWaveIdFileHook
        retn
    }
}

static void iterateAdxFiles(const char* path, const char* filter, std::vector<AdxFile>& adxFiles)
{
    WIN32_FIND_DATAA findFileData{};
    HANDLE hFindFile = FindFirstFileA(filter, &findFileData);
    if (hFindFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                char* endPtr = nullptr;
                int32_t id = strtol(findFileData.cFileName, &endPtr, 10);

                if (endPtr > findFileData.cFileName)
                {
                    auto& adxFile = adxFiles.emplace_back();
                    adxFile.id = id;
                    adxFile.size = findFileData.nFileSizeLow;
                    strcpy_s(adxFile.path, path);
                    strcat_s(adxFile.path, findFileData.cFileName);
                }
            }
        } while (FindNextFileA(hFindFile, &findFileData));
        FindClose(hFindFile);
    }
}

void Awb::init()
{
    WRITE_JUMP(0xCA8CB6, criAtomPlayerSetWaveIdFileTrampoline);
    iterateAdxFiles("bgm\\BGM000\\", "data\\bgm\\BGM000\\*.adx", s_adxFilesBgm000);
    iterateAdxFiles("bgm\\BGM001\\", "data\\bgm\\BGM001\\*.adx", s_adxFilesBgm001);
}
