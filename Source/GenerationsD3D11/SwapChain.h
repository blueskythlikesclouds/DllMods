#pragma once

class Device;
class RenderTargetSurface;

class SwapChain
{
public:
    virtual RenderTargetSurface* getRenderTargetSurface() = 0;

    virtual bool initialize(Device* device, D3DPRESENT_PARAMETERS* presentationParameters, DXGI_SCALING scaling) = 0;
    virtual void present(Device* device, UINT syncInterval) = 0;
};
