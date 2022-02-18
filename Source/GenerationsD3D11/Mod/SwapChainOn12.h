#pragma once

#include "SwapChain.h"

class RenderTargetTexture;

class SwapChainOn12 : public SwapChain
{
    ComPtr<ID3D12Device> d3d12Device;
    ComPtr<IDXGISwapChain3> swapChain;
    ComPtr<RenderTargetTexture> backBuffers[2];
    UINT backBufferIndex{};
    ComPtr<ID3D11Query> query;

public:
    RenderTargetSurface* getRenderTargetSurface() override;

    bool initialize(Device* device, D3DPRESENT_PARAMETERS* presentationParameters, DXGI_SCALING scaling) override;
    void present(Device* device, UINT syncInterval) override;
};
