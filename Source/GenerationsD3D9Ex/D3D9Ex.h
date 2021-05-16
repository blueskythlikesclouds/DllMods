#pragma once

#include "D3D9ExUnknown.h"

class D3D9ExDevice;

class D3D9Ex : public D3D9ExUnknown
{
    IDirect3D9Ex* d3dEx;

    // Workaround for configuration tool errors.
    IDirect3D9* d3d;

public:
    D3D9Ex(UINT SDKVersion);
    ~D3D9Ex();

    virtual HRESULT RegisterSoftwareDevice(void* pInitializeFunction) final;
    virtual UINT GetAdapterCount() final;
    virtual HRESULT GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) final;
    virtual UINT GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) final;
    virtual HRESULT EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) final;
    virtual HRESULT GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) final;
    virtual HRESULT CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) final;
    virtual HRESULT CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) final;
    virtual HRESULT CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat,BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels) final;
    virtual HRESULT CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) final;
    virtual HRESULT CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) final;
    virtual HRESULT GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) final;
    virtual HMONITOR GetAdapterMonitor(UINT Adapter) final;
    virtual HRESULT CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3D9ExDevice** ppReturnedDeviceInterface) final;
};

