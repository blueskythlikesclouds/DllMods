#include "Configuration.h"
#include "Context.h"
#include "ParameterEditor.h"
#include "Path.h"
#include "SynchronizedObject.h"

#include <LostCodeLoader.h>

#include "MaterialEditor.h"

SynchronizedObject** const APPLICATION_DOCUMENT = (SynchronizedObject**)0x1E66B34;

HOOK(LRESULT, __stdcall, WndProc, 0xE7B6C0, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Context::wndProc(hWnd, Msg, wParam, lParam))
        return true;

    return originalWndProc(hWnd, Msg, wParam, lParam);
}

extern "C" __declspec(dllexport) void OnFrame()
{
    const SynchronizedObject::Lock lock(*APPLICATION_DOCUMENT);

    if (!lock.getSuccess())
        return;

    const uint32_t applicationDocument = (uint32_t)lock.getObject();
    if (!applicationDocument)
        return;

    if (!Context::isInitialized())
    {
        const uint32_t application = *(uint32_t*)(applicationDocument + 20);
        if (!application)
            return;

        IUnknown* device = *(IUnknown**)(*(uint32_t*)(application + 80) + 8);
        if (!device)
            return;

        HWND window = *(HWND*)(application + 72);
        if (!window)
            return;

        Context::initialize(window, device);
    }

    const uint32_t parameterEditor = *(uint32_t*)(applicationDocument + 516);
    if (!parameterEditor)
        return;

    const uint32_t globalParameterManager = *(uint32_t*)(parameterEditor + 172);
    if (!globalParameterManager)
        return;

    ParameterEditor::updateParameters(
        (List<AbstractParameter*>*)(globalParameterManager + 4), (List<AbstractParameter*>*)(globalParameterManager + 16));

    Context::update();
}

extern "C" __declspec(dllexport) void Init(ModInfo* modInfo)
{
    Context::setModDirectoryPath(Path::getDirectoryPath(modInfo->CurrentMod->Path));

    Configuration::load();
    std::atexit(Configuration::save);

    INSTALL_HOOK(WndProc);

    if (Configuration::loadDebugShaders)
    {
        WRITE_MEMORY(0x1AD99D0, char*, "shader_debug.ar");
        WRITE_MEMORY(0x1AD99D4, char*, "shader_debug_add.ar");
        WRITE_MEMORY(0x1AD99E8, char*, "shader_debug.arl");
        WRITE_MEMORY(0x1AD99EC, char*, "shader_debug_add.arl");
    }
}