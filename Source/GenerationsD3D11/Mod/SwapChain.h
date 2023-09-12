#pragma once

class Device;
class RenderTargetSurface;

class SwapChain
{
private:
    ComPtr<IDXGISwapChain2> swapChain;
    ComPtr<RenderTargetSurface> renderTargetSurface;
    HANDLE waitHandle{};

public:
    RenderTargetSurface* getRenderTargetSurface() const;
    
    bool initialize(Device* device, D3DPRESENT_PARAMETERS* presentationParameters, DXGI_SCALING scaling);
    void present(Device* device, UINT syncInterval) const;
};
