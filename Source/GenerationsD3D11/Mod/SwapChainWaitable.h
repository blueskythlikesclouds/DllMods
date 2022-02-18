#pragma once

#include "SwapChain.h"

class SwapChainWaitable : public SwapChain
{
    ComPtr<IDXGISwapChain2> swapChain;
    ComPtr<RenderTargetSurface> renderTargetSurface;
    HANDLE waitHandle{};

public:
    RenderTargetSurface* getRenderTargetSurface() override;

    bool initialize(Device* device, D3DPRESENT_PARAMETERS* presentationParameters, DXGI_SCALING scaling) override;
    void present(Device* device, UINT syncInterval) override;
};
