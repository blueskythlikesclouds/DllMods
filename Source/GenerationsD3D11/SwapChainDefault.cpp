#include "SwapChainDefault.h"

#include "Device.h"
#include "RenderTargetTexture.h"
#include "TypeConverter.h"

RenderTargetSurface* SwapChainDefault::getRenderTargetSurface()
{
    return renderTargetSurface.Get();
}

bool SwapChainDefault::initialize(Device* device, D3DPRESENT_PARAMETERS* presentationParameters, DXGI_SCALING scaling)
{
    DXGI_SWAP_CHAIN_DESC desc{};
    desc.BufferDesc.Width = presentationParameters->BackBufferWidth;
    desc.BufferDesc.Height = presentationParameters->BackBufferHeight;
    desc.BufferDesc.Format = TypeConverter::convert(presentationParameters->BackBufferFormat);
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = presentationParameters->BackBufferCount;
    desc.OutputWindow = presentationParameters->hDeviceWindow;
    desc.Windowed = presentationParameters->Windowed;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    ComPtr<IDXGIFactory> dxgiFactory;
    CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));

    dxgiFactory->CreateSwapChain(device->get(), &desc, swapChain.GetAddressOf());
    swapChain->SetFullscreenState(TRUE, nullptr);

    ComPtr<ID3D11Texture2D> backBuffer;
    swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));

    ComPtr<RenderTargetTexture> renderTargetTexture;
    renderTargetTexture.Attach(new RenderTargetTexture(device, backBuffer.Get(), DXGI_FORMAT_UNKNOWN));
    renderTargetTexture->GetSurfaceLevel(0, reinterpret_cast<Surface**>(renderTargetSurface.GetAddressOf()));

    return true;
}

void SwapChainDefault::present(Device* device, UINT syncInterval)
{
    swapChain->Present(syncInterval, 0);
}
