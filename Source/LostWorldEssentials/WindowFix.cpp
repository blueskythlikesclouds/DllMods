#include "WindowFix.h"

HOOK(void, __fastcall, SetWindow, ASLR(0x401660), void* This, void* _, void* A2, bool fullscreen)
{
    originalSetWindow(This, _, A2, fullscreen);

    if (!fullscreen)
    {
        const HWND hWnd = *reinterpret_cast<HWND*>(static_cast<uint8_t*>(This) + 0xE4);

        RECT windowRect, clientRect;
        GetWindowRect(hWnd, &windowRect);
        GetClientRect(hWnd, &clientRect);

        LONG windowWidth = windowRect.right - windowRect.left;
        LONG windowHeight = windowRect.bottom - windowRect.top;

        const LONG clientWidth = clientRect.right - clientRect.left;
        const LONG clientHeight = clientRect.bottom - clientRect.top;

        const LONG deltaX = windowWidth - clientWidth;
        const LONG deltaY = windowHeight - clientHeight;

        LONG windowX = windowRect.left - (deltaX / 2);
        LONG windowY = windowRect.right - (deltaY / 2);
        windowWidth += deltaX;
        windowHeight += deltaY;

        const HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO monitorInfo{ sizeof(monitorInfo) };
        if (GetMonitorInfoA(hMonitor, &monitorInfo))
        {
            LONG curWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
            LONG curHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

            windowX = monitorInfo.rcWork.left;
            windowY = monitorInfo.rcWork.top;

            if (windowWidth > curWidth)
            {
                curWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
                windowX = monitorInfo.rcMonitor.left;
            }

            if (windowHeight > curHeight)
            {
                curHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
                windowY = monitorInfo.rcMonitor.top;
            }

            windowX += (curWidth - windowWidth) / 2;
            windowY += (curHeight - windowHeight) / 2;
        }

        SetWindowPos(
            hWnd,
            HWND_TOP,
            windowX,
            windowY,
            windowWidth,
            windowHeight,
            SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
    }
}

void WindowFix::init()
{
    INSTALL_HOOK(SetWindow);
}
