#include "D3D9Ex.h"
#include "D3D9ExDevice.h"

D3D9Ex::D3D9Ex(UINT SDKVersion) : d3dEx(nullptr)
{
    Direct3DCreate9Ex(D3D_SDK_VERSION, &d3dEx);
}

D3D9Ex::~D3D9Ex()
{
    d3dEx->Release();
}

HRESULT D3D9Ex::RegisterSoftwareDevice(void* pInitializeFunction)
{
    return d3dEx->RegisterSoftwareDevice(pInitializeFunction);
}

UINT D3D9Ex::GetAdapterCount() 
{
    return d3dEx->GetAdapterCount();
}

HRESULT D3D9Ex::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) 
{
    return d3dEx->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT D3D9Ex::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) 
{
    return d3dEx->GetAdapterModeCount(Adapter, Format);
}

HRESULT D3D9Ex::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) 
{
    return d3dEx->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

HRESULT D3D9Ex::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) 
{
    return d3dEx->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT D3D9Ex::CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) 
{
    return d3dEx->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
}

HRESULT D3D9Ex::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) 
{
    return d3dEx->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

HRESULT D3D9Ex::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) 
{
    return d3dEx->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

HRESULT D3D9Ex::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) 
{
    return d3dEx->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

HRESULT D3D9Ex::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) 
{
    return d3dEx->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

HRESULT D3D9Ex::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) 
{
    return d3dEx->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HMONITOR D3D9Ex::GetAdapterMonitor(UINT Adapter) 
{
    return d3dEx->GetAdapterMonitor(Adapter);
}

HRESULT D3D9Ex::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3D9ExDevice** ppReturnedDeviceInterface) 
{
    pPresentationParameters->Windowed = *(uint8_t*)0xA5EB5B != 0x89; // Check if Borderless Fullscreen patch is enabled in HMM

    if (pPresentationParameters->Windowed)
        pPresentationParameters->FullScreen_RefreshRateInHz = 0;

    D3DDISPLAYMODEEX displayMode;
    ZeroMemory(&displayMode, sizeof(D3DDISPLAYMODEEX));

    displayMode.Size = sizeof(D3DDISPLAYMODEEX);
    displayMode.Width = pPresentationParameters->BackBufferWidth;
    displayMode.Height = pPresentationParameters->BackBufferHeight;
    displayMode.RefreshRate = pPresentationParameters->FullScreen_RefreshRateInHz;
    displayMode.Format = pPresentationParameters->BackBufferFormat;

    IDirect3DDevice9Ex* d3dDevice;

    HRESULT result = d3dEx->CreateDeviceEx(
        Adapter,
        DeviceType,
        hFocusWindow,
        BehaviorFlags,
        pPresentationParameters, !pPresentationParameters->Windowed ? &displayMode : nullptr, 
        &d3dDevice);

    if (FAILED(result))
        return result;

    *ppReturnedDeviceInterface = new D3D9ExDevice(this, d3dDevice);

    d3dDevice->Release();
    return result;
}