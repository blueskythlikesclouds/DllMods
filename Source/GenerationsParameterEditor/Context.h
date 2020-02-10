#pragma once

#include <d3d9.h>

class Context
{
    static std::string modDirectoryPath;
    static std::string imGuiIniPath;

    static HWND window;
    static IDirect3DDevice9* device;

    static ImFont* font;

public:
    static std::string getModDirectoryPath();
    static void setModDirectoryPath(const std::string& value);

    static bool isInitialized();

    static void initialize(HWND window, IDirect3DDevice9* device);

    static void update();
    static void reset();

    static LRESULT wndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
};
