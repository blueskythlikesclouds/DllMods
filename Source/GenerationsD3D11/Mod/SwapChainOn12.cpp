#include "SwapChainOn12.h"

#include "RenderTargetSurface.h"
#include "TypeConverter.h"
#include "RenderTargetTexture.h"

RenderTargetSurface* SwapChainOn12::getRenderTargetSurface()
{
    RenderTargetSurface* surface;
    backBuffers[backBufferIndex]->GetSurfaceLevel(0, reinterpret_cast<Surface**>(&surface));

    return surface;
}

bool SwapChainOn12::initialize(Device* device, D3DPRESENT_PARAMETERS* presentationParameters, DXGI_SCALING scaling)
{
#if _DEBUG
    ComPtr<ID3D12Debug> debugInterface;
    D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
    debugInterface->EnableDebugLayer();
#endif

    D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(d3d12Device.GetAddressOf()));

    D3D12_COMMAND_QUEUE_DESC desc;
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    ComPtr<ID3D12CommandQueue> commandQueue;
    d3d12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue));

    ComPtr<IDXGIFactory4> dxgiFactory;
    CreateDXGIFactory2(
#if _DEBUG
        DXGI_CREATE_FACTORY_DEBUG,
#else
        0,
#endif
        IID_PPV_ARGS(dxgiFactory.GetAddressOf()));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    swapChainDesc.Width = presentationParameters->BackBufferWidth;
    swapChainDesc.Height = presentationParameters->BackBufferHeight;
    swapChainDesc.Format = TypeConverter::convert(presentationParameters->BackBufferFormat);
    swapChainDesc.Stereo = false;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Scaling = scaling;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags = 0;

    ComPtr<IDXGISwapChain1> swapChain1;
    dxgiFactory->CreateSwapChainForHwnd(
        commandQueue.Get(),
        presentationParameters->hDeviceWindow,
        &swapChainDesc,
        nullptr,
        nullptr,
        swapChain1.GetAddressOf());

    swapChain1.As(&swapChain);

    for (size_t i = 0; i < _countof(backBuffers); i++)
    {
        ComPtr<ID3D12Resource> backBuffer;
        swapChain->GetBuffer(i, IID_PPV_ARGS(backBuffer.GetAddressOf()));

        wchar_t name[32];
        swprintf_s(name, L"%p", backBuffer.Get());

        HANDLE handle;
        d3d12Device->CreateSharedHandle(backBuffer.Get(), nullptr, GENERIC_ALL, name, &handle);

        ComPtr<ID3D11Texture2D> texture;
        reinterpret_cast<ID3D11Device1*>(device->get())->OpenSharedResource1(handle, IID_PPV_ARGS(texture.GetAddressOf()));

        backBuffers[i].Attach(new RenderTargetTexture(device, texture.Get(), DXGI_FORMAT_UNKNOWN));
    }

    backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    D3D11_QUERY_DESC queryDesc{};
    queryDesc.Query = D3D11_QUERY_EVENT;
    device->get()->CreateQuery(&queryDesc, query.GetAddressOf());

    return true;
}

void SwapChainOn12::present(Device* device, UINT syncInterval)
{
    device->getContext()->Flush();
    device->getContext()->End(query.Get());
    device->getContext()->Flush();

    while (device->getContext()->GetData(query.Get(), nullptr, 0, 0));
    
    swapChain->Present(syncInterval, DXGI_PRESENT_DO_NOT_WAIT);
    backBufferIndex = swapChain->GetCurrentBackBufferIndex();
}
