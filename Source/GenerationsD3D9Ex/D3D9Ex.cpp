#include "D3D9Ex.h"
#include "D3D9ExDevice.h"

D3D9Ex::D3D9Ex(UINT SDKVersion) : d3d(Direct3DCreate9(D3D_SDK_VERSION))
{
    Direct3DCreate9Ex(D3D_SDK_VERSION, &d3dEx);
}

D3D9Ex::~D3D9Ex()
{
    d3d->Release();
    d3dEx->Release();
}

HRESULT D3D9Ex::RegisterSoftwareDevice(void* pInitializeFunction)
{
    return d3d->RegisterSoftwareDevice(pInitializeFunction);
}

UINT D3D9Ex::GetAdapterCount() 
{
    return d3d->GetAdapterCount();
}

HRESULT D3D9Ex::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) 
{
    return d3d->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT D3D9Ex::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) 
{
    return d3d->GetAdapterModeCount(Adapter, Format);
}

HRESULT D3D9Ex::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) 
{
    return d3d->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

HRESULT D3D9Ex::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) 
{
    return d3d->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT D3D9Ex::CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) 
{
    return d3d->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
}

HRESULT D3D9Ex::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) 
{
    return d3d->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

HRESULT D3D9Ex::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) 
{
    return d3d->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

HRESULT D3D9Ex::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) 
{
    return d3d->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

HRESULT D3D9Ex::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) 
{
    return d3d->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

HRESULT D3D9Ex::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) 
{
    return d3d->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HMONITOR D3D9Ex::GetAdapterMonitor(UINT Adapter) 
{
    return d3d->GetAdapterMonitor(Adapter);
}

HRESULT D3D9Ex::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3D9ExDevice** ppReturnedDeviceInterface) 
{
    pPresentationParameters->Windowed = *(uint8_t*)0xA5EB5B != 0x89; // Check if Borderless Fullscreen patch is enabled in HMM

    D3DPRESENT_PARAMETERS presentationParameters;
    ZeroMemory(&presentationParameters, sizeof(D3DPRESENT_PARAMETERS));

    // I'm not sure about any of the things here, please don't kill me for it.
    presentationParameters.BackBufferWidth = pPresentationParameters->BackBufferWidth;
    presentationParameters.BackBufferHeight = pPresentationParameters->BackBufferHeight;
    presentationParameters.BackBufferCount = 3;
    presentationParameters.hDeviceWindow = hFocusWindow;
    presentationParameters.SwapEffect = D3DSWAPEFFECT_FLIPEX;
    presentationParameters.Windowed = pPresentationParameters->Windowed;
    presentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    presentationParameters.BackBufferFormat = D3DFMT_A8R8G8B8;
    presentationParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
    presentationParameters.MultiSampleQuality = 0;
    presentationParameters.EnableAutoDepthStencil = true;
    presentationParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
    presentationParameters.Flags = 0;
    presentationParameters.FullScreen_RefreshRateInHz = !pPresentationParameters->Windowed ? pPresentationParameters->FullScreen_RefreshRateInHz : 0;

    D3DDISPLAYMODEEX displayMode;
    ZeroMemory(&displayMode, sizeof(D3DDISPLAYMODEEX));

    displayMode.Size = sizeof(D3DDISPLAYMODEEX);
    displayMode.Width = pPresentationParameters->BackBufferWidth;
    displayMode.Height = pPresentationParameters->BackBufferHeight;
    displayMode.RefreshRate = pPresentationParameters->FullScreen_RefreshRateInHz;
    displayMode.Format = D3DFMT_A8R8G8B8;

    IDirect3DDevice9Ex* d3dDevice;

    HRESULT result = d3dEx->CreateDeviceEx(
        Adapter,
        DeviceType,
        hFocusWindow,
        D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_PUREDEVICE,
        &presentationParameters, !pPresentationParameters->Windowed ? &displayMode : nullptr, 
        &d3dDevice);

    if (FAILED(result))
        return result;

    *ppReturnedDeviceInterface = new D3D9ExDevice(this, d3dDevice);

    d3dDevice->Release();
    return result;
}