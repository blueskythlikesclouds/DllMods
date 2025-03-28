#pragma once

class Device;
class RenderTargetSurface;

class SwapChain
{
private:
    ComPtr<IDXGISwapChain2> swapChain;
    ComPtr<RenderTargetSurface> renderTargetSurface;
    HANDLE waitHandle{};
    bool pendingWait{ true };

public:
    RenderTargetSurface* getRenderTargetSurface() const;
    
    bool initialize(Device* device, D3DPRESENT_PARAMETERS* presentationParameters, DXGI_SCALING scaling);
    void wait();
    void present(Device* device, UINT syncInterval);
};
