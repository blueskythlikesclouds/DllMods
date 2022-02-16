#pragma once

#include "SwapChain.h"

class SwapChainDefault : public SwapChain
{
    ComPtr<IDXGISwapChain> swapChain;
    ComPtr<RenderTargetSurface> renderTargetSurface;

public:
    RenderTargetSurface* getRenderTargetSurface() override;

    bool initialize(Device* device, D3DPRESENT_PARAMETERS* presentationParameters, DXGI_SCALING scaling) override;
    void present(Device* device, UINT syncInterval) override;
};
