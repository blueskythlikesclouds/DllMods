#include "Configuration.h"
#include "D3D9Ex.h"
#include "D3D9ExDevice.h"

D3D9Ex::D3D9Ex(UINT SDKVersion) : d3dEx(nullptr), d3d(Configuration::useConfigWorkaround ? Direct3DCreate9(D3D_SDK_VERSION) : nullptr)
{
    Direct3DCreate9Ex(D3D_SDK_VERSION, &d3dEx);
}

D3D9Ex::~D3D9Ex()
{
    d3dEx->Release();

    if (d3d)
        d3d->Release();
}

HRESULT D3D9Ex::RegisterSoftwareDevice(void* pInitializeFunction)
{
    return (Configuration::useConfigWorkaround ? d3d : d3dEx)->RegisterSoftwareDevice(pInitializeFunction);
}

UINT D3D9Ex::GetAdapterCount() 
{
    return (Configuration::useConfigWorkaround ? d3d : d3dEx)->GetAdapterCount();
}

HRESULT D3D9Ex::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) 
{
    return (Configuration::useConfigWorkaround ? d3d : d3dEx)->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT D3D9Ex::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) 
{
    return (Configuration::useConfigWorkaround ? d3d : d3dEx)->GetAdapterModeCount(Adapter, Format);
}

HRESULT D3D9Ex::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) 
{
    return (Configuration::useConfigWorkaround ? d3d : d3dEx)->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

HRESULT D3D9Ex::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) 
{
    return (Configuration::useConfigWorkaround ? d3d : d3dEx)->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT D3D9Ex::CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) 
{
    return (Configuration::useConfigWorkaround ? d3d : d3dEx)->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
}

HRESULT D3D9Ex::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) 
{
    return (Configuration::useConfigWorkaround ? d3d : d3dEx)->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

HRESULT D3D9Ex::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) 
{
    return (Configuration::useConfigWorkaround ? d3d : d3dEx)->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

HRESULT D3D9Ex::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) 
{
    return (Configuration::useConfigWorkaround ? d3d : d3dEx)->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

HRESULT D3D9Ex::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) 
{
    return (Configuration::useConfigWorkaround ? d3d : d3dEx)->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

HRESULT D3D9Ex::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) 
{
    return (Configuration::useConfigWorkaround ? d3d : d3dEx)->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HMONITOR D3D9Ex::GetAdapterMonitor(UINT Adapter) 
{
    return (Configuration::useConfigWorkaround ? d3d : d3dEx)->GetAdapterMonitor(Adapter);
}

HRESULT D3D9Ex::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3D9ExDevice** ppReturnedDeviceInterface) 
{
    DisplayMode displayMode = Configuration::displayMode;
    if (displayMode != DisplayMode::WINDOWED && *(uint8_t*)0xA5EB5B != 0x89) // Check if Borderless Fullscreen patch is enabled in HMM
        displayMode = DisplayMode::BORDERLESS_FULLSCREEN;

    LONG_PTR windowStyle = GetWindowLong(pPresentationParameters->hDeviceWindow, GWL_STYLE);

    MONITORINFO monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFO);

    GetMonitorInfo((Configuration::useConfigWorkaround ? d3d : d3dEx)->GetAdapterMonitor(Adapter), &monitorInfo);

    LONG x, y, width, height;

    const LONG backBufferWidth = static_cast<LONG>(pPresentationParameters->BackBufferWidth);
    const LONG backBufferHeight = static_cast<LONG>(pPresentationParameters->BackBufferHeight);

    if (displayMode == DisplayMode::BORDERLESS || displayMode == DisplayMode::WINDOWED)
    {
        width = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
        height = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

        x = monitorInfo.rcWork.left;
        y = monitorInfo.rcWork.top;

        if (backBufferWidth > width)
        {
            width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
            x = monitorInfo.rcMonitor.left;
        }

        if (backBufferHeight > height)
        {
            height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
            y = monitorInfo.rcMonitor.top;
        }

        x += (width - backBufferWidth) / 2;
        y += (height - backBufferHeight) / 2;

        width = backBufferWidth;
        height = backBufferHeight;

        if (displayMode == DisplayMode::WINDOWED)
        {
            windowStyle = WS_OVERLAPPEDWINDOW;
            if (!Configuration::allowResizeInWindowed)
                windowStyle &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
        }
        else
            windowStyle = WS_POPUP;
    }
    else
    {
        x = monitorInfo.rcMonitor.left;
        y = monitorInfo.rcMonitor.top;
        width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    }

    if (displayMode == DisplayMode::WINDOWED)
        ShowCursor(true);

    // Force the window to the foreground.
    const DWORD windowThreadProcessId = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
    const DWORD currentThreadId = GetCurrentThreadId();
    AttachThreadInput(windowThreadProcessId, currentThreadId, TRUE);
    BringWindowToTop(pPresentationParameters->hDeviceWindow);
    ShowWindow(pPresentationParameters->hDeviceWindow, SW_SHOW);
    AttachThreadInput(windowThreadProcessId, currentThreadId, FALSE);

    SetWindowLongPtr(pPresentationParameters->hDeviceWindow, GWL_STYLE, WS_VISIBLE | windowStyle);
    SetWindowPos(pPresentationParameters->hDeviceWindow, HWND_TOP, x, y, width, height, SWP_FRAMECHANGED);

    // In windowed, title bar and border are included when setting width/height. Let's fix that and not be like Lost World/Forces.
    if (displayMode == DisplayMode::WINDOWED)
    {
        RECT windowRect, clientRect;
        GetWindowRect(pPresentationParameters->hDeviceWindow, &windowRect);
        GetClientRect(pPresentationParameters->hDeviceWindow, &clientRect);

        const LONG windowWidth = windowRect.right - windowRect.left;
        const LONG windowHeight = windowRect.bottom - windowRect.top;

        const LONG clientWidth = clientRect.right - clientRect.left;
        const LONG clientHeight = clientRect.bottom - clientRect.top;

        const LONG deltaX = windowWidth - clientWidth;
        const LONG deltaY = windowHeight - clientHeight;

        SetWindowPos(
            pPresentationParameters->hDeviceWindow,
            HWND_TOP,
            x - deltaX / 2,
            y - deltaY / 2,
            width + deltaX,
            height + deltaY,
            SWP_FRAMECHANGED);
    }

    pPresentationParameters->Windowed = displayMode != DisplayMode::FULLSCREEN; 

    if (pPresentationParameters->Windowed)
        pPresentationParameters->FullScreen_RefreshRateInHz = 0;

    if (Configuration::enable10BitOutput)
        pPresentationParameters->BackBufferFormat = D3DFMT_A2R10G10B10;

    D3DDISPLAYMODEEX displayModeEx;
    ZeroMemory(&displayModeEx, sizeof(D3DDISPLAYMODEEX));

    displayModeEx.Size = sizeof(D3DDISPLAYMODEEX);
    displayModeEx.Width = pPresentationParameters->BackBufferWidth;
    displayModeEx.Height = pPresentationParameters->BackBufferHeight;
    displayModeEx.RefreshRate = pPresentationParameters->FullScreen_RefreshRateInHz;
    displayModeEx.Format = pPresentationParameters->BackBufferFormat;

    IDirect3DDevice9Ex* d3dDevice;

    HRESULT result = d3dEx->CreateDeviceEx(
        Adapter,
        DeviceType,
        hFocusWindow,
        BehaviorFlags,
        pPresentationParameters, !pPresentationParameters->Windowed ? &displayModeEx : nullptr, 
        &d3dDevice);

    if (FAILED(result))
        return result;

    *ppReturnedDeviceInterface = new D3D9ExDevice(this, d3dDevice);

    d3dDevice->Release();
    return result;
}