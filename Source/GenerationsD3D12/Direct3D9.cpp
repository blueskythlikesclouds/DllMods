#include "Direct3D9.h"
#include "Device.h"

Direct3D9::Direct3D9(UINT SDKVersion)
{
    d3d9 = Direct3DCreate9(SDKVersion);
}

Direct3D9::~Direct3D9()
{
    d3d9->Release();
}

FUNCTION_STUB(HRESULT, Direct3D9::RegisterSoftwareDevice, void* pInitializeFunction)

UINT Direct3D9::GetAdapterCount()
{
    return d3d9->GetAdapterCount();
}

HRESULT Direct3D9::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier)
{
    return d3d9->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT Direct3D9::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format)
{
    return d3d9->GetAdapterModeCount(Adapter, Format);
}

HRESULT Direct3D9::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
{
    return d3d9->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

HRESULT Direct3D9::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode)
{
    return d3d9->GetAdapterDisplayMode(Adapter, pMode);
}

FUNCTION_STUB(HRESULT, Direct3D9::CheckDeviceType, UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed)

HRESULT Direct3D9::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
    return d3d9->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

FUNCTION_STUB(HRESULT, Direct3D9::CheckDeviceMultiSampleType, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)

FUNCTION_STUB(HRESULT, Direct3D9::CheckDepthStencilMatch, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)

FUNCTION_STUB(HRESULT, Direct3D9::CheckDeviceFormatConversion, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat)

HRESULT Direct3D9::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps)
{
    return d3d9->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

FUNCTION_STUB(HMONITOR, Direct3D9::GetAdapterMonitor, UINT Adapter)

HRESULT Direct3D9::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, Device** ppReturnedDeviceInterface)
{
    *ppReturnedDeviceInterface = new Device(pPresentationParameters);
    return S_OK;
}