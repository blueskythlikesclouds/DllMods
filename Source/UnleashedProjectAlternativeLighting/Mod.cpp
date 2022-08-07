struct Stage
{
    std::string_view bb;
    std::string_view swa;
    std::string_view proxy;
    std::string_view bbAdd;
    std::string_view swaAdd;
};

#define STAGE(x, y) \
    { x, y, "PROXY_"y"_PROXY", "Packed/"x"/Stage-Add.pfd", "Additional/"y"/Stage-Add.pfd" }

const Stage stages[] =
{
    STAGE("ghz100", "ActD_MykonosAct1"),
    STAGE("ghz200", "ActD_MykonosAct2"),
    STAGE("cpz200", "ActD_Africa"),
    STAGE("pla200", "ActD_MykonosAct1")
};

HOOK(bool, __stdcall, CApplicationDocumentParseArchiveTree, 0xD4C8E0, void* A1, char* data, const size_t size, void* database)
{
    std::stringstream dst;

    for (auto& stage : stages)
    {
        dst <<
            "<DefAppend>"
            "<Name>" << stage.bb << "</Name>"
            "<Archive>" << stage.proxy << "</Archive>"
            "</DefAppend>";
    }

    const std::string str = dst.str();
    const size_t newSize = size + str.size();
    const std::unique_ptr<char[]> buffer = std::make_unique<char[]>(newSize);
    memcpy(buffer.get(), data, size);

    char* insertionPos = strstr(buffer.get(), "<Include>");

    memmove(insertionPos + str.size(), insertionPos, size - (size_t)(insertionPos - buffer.get()));
    memcpy(insertionPos, str.data(), str.size());

    bool result;
    {
        result = originalCApplicationDocumentParseArchiveTree(A1, buffer.get(), newSize, database);
    }

    return result;
}

void replaceStr(std::string& str, const std::string_view& src, const std::string_view& dst)
{
    size_t pos = 0;
    while ((pos = str.find(src, pos)) != std::string::npos)
    {
        str.replace(pos, src.size(), dst);
        pos += dst.size();
    }
}

hh::base::CSharedString resolveFilePath(const hh::base::CSharedString& in_rPath)
{
    std::string str = in_rPath.c_str();

    for (auto& stage : stages)
    {
        replaceStr(str, stage.bbAdd, stage.swaAdd);
        replaceStr(str, stage.bb, stage.swa);
        replaceStr(str, stage.proxy, stage.bb);
    }

    return str.c_str();
}

HOOK(bool, __fastcall, CFileReaderCriD3D9Open, 0x6A03B0, void* This, void* Edx, const hh::base::CSharedString& filePath)
{
    return originalCFileReaderCriD3D9Open(This, Edx, resolveFilePath(filePath));
}

HOOK(bool, __fastcall, CFileBinderCriExists, 0x66A140, void* This, void* Edx, const hh::base::CSharedString& filePath)
{
    return originalCFileBinderCriExists(This, Edx, resolveFilePath(filePath));
}

extern "C" __declspec(dllexport) void Init()
{
    INSTALL_HOOK(CApplicationDocumentParseArchiveTree);
    INSTALL_HOOK(CFileReaderCriD3D9Open);
    INSTALL_HOOK(CFileBinderCriExists);
}