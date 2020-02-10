#include "Path.h"

std::string Path::getDirectoryPath(const std::string& path)
{
    const size_t pos = path.find_last_of("\\/");
    return path.substr(0, pos != std::string::npos ? pos : 0);
}

std::string Path::getGameDirectoryPath()
{
    WCHAR moduleFilePathWideChar[1024];
    GetModuleFileNameW(NULL, moduleFilePathWideChar, 1024);

    CHAR moduleFilePathMultiByte[1024];
    WideCharToMultiByte(CP_UTF8, 0, moduleFilePathWideChar, -1, moduleFilePathMultiByte, 1024, 0, 0);

    return getDirectoryPath(moduleFilePathMultiByte);
}
