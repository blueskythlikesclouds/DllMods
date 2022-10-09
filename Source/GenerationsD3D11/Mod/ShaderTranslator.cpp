#include "ShaderTranslator.h"

namespace
{
    template<typename T>
    void getProcAddress(const HMODULE module, const char* name, T*& destination)
    {
        destination = (T*)GetProcAddress(module, name);
    }
}

void ShaderTranslator::init(const std::string& dir)
{
    if (!translate)
    {
        const auto filePathMultiByte = dir + "/ShaderTranslator.Bridge.dll";

        wchar_t filePathWideChar[0x400];
        MultiByteToWideChar(CP_UTF8, 0, filePathMultiByte.c_str(), -1, filePathWideChar, _countof(filePathWideChar));

        const auto module = LoadLibraryW(filePathWideChar);
        if (!module)
        {
            MessageBoxW(0, L"NO!!!!!!!!!!!!!!!!", 0, 0);
        }

        getProcAddress(module, "RegisterOnProcessExit", registerOnProcessExit);
        getProcAddress(module, "Translate", translate);
        getProcAddress(module, "Get", get);
        getProcAddress(module, "Free", free);
        getProcAddress(module, "Version", version);
    }
}