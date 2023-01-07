#pragma once

#include <d3d9.h>

class Context
{
    static std::string modDirectoryPath;
    static std::string imGuiIniPath;

    static HWND window;
    static IUnknown* device;

    static ImFont* font;

public:
    static std::string getModDirectoryPath();
    static void setModDirectoryPath(const std::string& value);

    static bool isInitialized();
    static void initialize(HWND window, IUnknown* device);

    static void update();

    static LRESULT wndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
};
