#include "Device.h"

#include "DepthStencilSurface.h"
#include "DepthStencilTexture.h"
#include "IndexBuffer.h"
#include "RenderTargetSurface.h"
#include "RenderTargetTexture.h"
#include "Shader.h"
#include "Surface.h"
#include "Texture.h"
#include "TypeConverter.h"
#include "VertexBuffer.h"
#include "VertexDeclaration.h"

UINT calculateIndexCount(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount)
{
    UINT vertexCount = 0;
    switch (PrimitiveType)
    {
    case D3DPT_POINTLIST:
        vertexCount = PrimitiveCount;
        break;
    case D3DPT_LINELIST:
        vertexCount = PrimitiveCount * 2;
        break;
    case D3DPT_LINESTRIP:
        vertexCount = PrimitiveCount + 1;
        break;
    case D3DPT_TRIANGLELIST:
        vertexCount = PrimitiveCount * 3;
        break;
    case D3DPT_TRIANGLESTRIP:
        vertexCount = PrimitiveCount + 2;
        break;
    case D3DPT_TRIANGLEFAN:
        vertexCount = PrimitiveCount + 2;
        break;
    }
    return vertexCount;
}

void Device::updatePipelineState()
{
    // submit when changing render targets or textures
    /*if (dirtyState[(size_t)DirtyStateIndex::RenderTarget] || dirtyState[(size_t)DirtyStateIndex::Texture])
        submitAll();*/

    ID3D12GraphicsCommandList* commandList = renderQueue.getCommandList();

    if (dirtyState[(size_t)DirtyStateIndex::RootSignature])
    {
        commandList->SetGraphicsRootSignature(rootSignature.Get());
        updateDirty(pso.pRootSignature, rootSignature.Get(), DirtyStateIndex::PipelineState);
    }

    if (dirtyState[(size_t)DirtyStateIndex::RenderTarget])
    {
        if (renderTargets[0] == backBufferRenderTargets[!backBufferIndex])
            renderTargets[0] = backBufferRenderTargets[backBufferIndex];

        D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDescriptorHandles[_countof(renderTargets)];

        size_t i;
        for (i = 0; i < _countof(renderTargets) && renderTargets[i]; i++)
        {
            renderTargetDescriptorHandles[i] = renderTargets[i]->getRtvDescriptorHandle();
            renderTargets[i]->transition(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

            // See if this render target is bound to any textures (that's illegal!)
            for (size_t j = 0; j < _countof(textures); j++)
            {
                if (textures[j] != renderTargets[i])
                    continue;

                textures[j] = nullptr;
                dirtyState.set((size_t)DirtyStateIndex::Texture);
            }
        }

        updateDirty(pso.NumRenderTargets, i, DirtyStateIndex::PipelineState);

        if (depthStencil)
        {
            depthStencil->transition(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);

            // See if this depth stencil is bound to any textures (just like above)
            for (size_t j = 0; j < _countof(textures); j++)
            {
                if (textures[j] != depthStencil)
                    continue;

                textures[j] = nullptr;
                dirtyState.set((size_t)DirtyStateIndex::Texture);
            }
        }

        commandList->OMSetRenderTargets(i, renderTargetDescriptorHandles, FALSE, depthStencil ? &depthStencil->getDsvDescriptorHandle() : nullptr);
    }

    if (dirtyState[(size_t)DirtyStateIndex::Viewport])
        commandList->RSSetViewports(1, &viewport);

    if (dirtyState[(size_t)DirtyStateIndex::AlphaTest])
        commandList->SetGraphicsRoot32BitConstants(4, 2, &alphaTestEnable, 0);

    if (dirtyState[(size_t)DirtyStateIndex::Texture])
    {
        const DescriptorHeap descriptorHeap = viewPool.allocate(device.Get(), 16);

        for (size_t i = 0; i < _countof(textures); i++)
        {
            if (!textures[i])
                continue;

            // Try for render target and depth stencil
            textures[i]->transition(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            textures[i]->transition(D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

            device->CreateShaderResourceView(
                textures[i]->getResource(),
                &textures[i]->getSrvDesc(),
                CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeap.cpuDescriptorHandle, i, descriptorHeap.incrementSize));
        }

        commandList->SetDescriptorHeaps(1, &descriptorHeap.descriptorHeap);
        commandList->SetGraphicsRootDescriptorTable(2, descriptorHeap.gpuDescriptorHandle);
    }

    if (dirtyState[(size_t)DirtyStateIndex::Sampler])
    {
        const DescriptorHeap descriptorHeap = samplerPool.allocate(device.Get(), 16);

        for (size_t i = 0; i < _countof(samplers); i++)
        {
            if (textures[i])
            {
                if (textures[i]->getFormat() == DXGI_FORMAT_D24_UNORM_S8_UINT)
                    samplers[i].Filter = (D3D12_FILTER)(samplers[i].Filter | 0x80);

                device->CreateSampler(&samplers[i], CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeap.cpuDescriptorHandle, i, descriptorHeap.incrementSize));

                if (textures[i]->getFormat() == DXGI_FORMAT_D24_UNORM_S8_UINT)
                    samplers[i].Filter = (D3D12_FILTER)(samplers[i].Filter & ~0x80);
            }
        }

        commandList->SetDescriptorHeaps(1, &descriptorHeap.descriptorHeap);
        commandList->SetGraphicsRootDescriptorTable(3, descriptorHeap.gpuDescriptorHandle);
    }

    if (dirtyState[(size_t)DirtyStateIndex::ScissorRect])
        commandList->RSSetScissorRects(1, &scissorRect);

    if (dirtyState[(size_t)DirtyStateIndex::PrimitiveTopology])
        commandList->IASetPrimitiveTopology(primitiveTopology);

    if (dirtyState[(size_t)DirtyStateIndex::VertexDeclaration] && vertexDeclaration)
        updateDirty(pso.InputLayout, vertexDeclaration->getInputLayoutDesc(), DirtyStateIndex::PipelineState);

    if (dirtyState[(size_t)DirtyStateIndex::VertexShader] && vertexShader)
        updateDirty(pso.VS, vertexShader->getShaderByteCode(), DirtyStateIndex::PipelineState);

    if (dirtyState[(size_t)DirtyStateIndex::VertexConstant])
    {
        const UploadBuffer uploadBuffer = uploadBufferPool.allocate(allocator.Get(), sizeof(vertexConstants));
        memcpy(uploadBuffer.data, &vertexConstants, sizeof(vertexConstants));

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
        cbvDesc.BufferLocation = uploadBuffer.gpuVirtualAddress;
        cbvDesc.SizeInBytes = uploadBuffer.size;

        const DescriptorHeap descriptorHeap = viewPool.allocate(device.Get(), 1);
        device->CreateConstantBufferView(&cbvDesc, descriptorHeap.cpuDescriptorHandle);

        commandList->SetDescriptorHeaps(1, &descriptorHeap.descriptorHeap);
        commandList->SetGraphicsRootDescriptorTable(0, descriptorHeap.gpuDescriptorHandle);
    }

    if (dirtyState[(size_t)DirtyStateIndex::VertexBuffer])
    {
        size_t i;
        for (i = 0; i < _countof(vertexBufferViews) && vertexBufferViews[i].BufferLocation; i++)
            ;

        commandList->IASetVertexBuffers(0, i, vertexBufferViews);
    }

    if (dirtyState[(size_t)DirtyStateIndex::IndexBuffer] && indexBuffer)
        commandList->IASetIndexBuffer(&indexBuffer->getIndexBufferView());

    if (dirtyState[(size_t)DirtyStateIndex::PixelShader] && pixelShader)
        updateDirty(pso.PS, pixelShader->getShaderByteCode(), DirtyStateIndex::PipelineState);

    if (dirtyState[(size_t)DirtyStateIndex::PixelConstant])
    {
        const UploadBuffer uploadBuffer = uploadBufferPool.allocate(allocator.Get(), sizeof(pixelConstants));
        memcpy(uploadBuffer.data, &pixelConstants, sizeof(pixelConstants));

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
        cbvDesc.BufferLocation = uploadBuffer.gpuVirtualAddress;
        cbvDesc.SizeInBytes = uploadBuffer.size;

        const DescriptorHeap descriptorHeap = viewPool.allocate(device.Get(), 1);
        device->CreateConstantBufferView(&cbvDesc, descriptorHeap.cpuDescriptorHandle);

        commandList->SetDescriptorHeaps(1, &descriptorHeap.descriptorHeap);
        commandList->SetGraphicsRootDescriptorTable(1, descriptorHeap.gpuDescriptorHandle);
    }

    if (dirtyState[(size_t)DirtyStateIndex::PipelineState])
    {
        const size_t hash = generateCrc32Hash(0, &pso, sizeof(pso));

        ComPtr<ID3D12PipelineState> pipelineState;

        const auto pair = psoMap.find(hash);

        if (pair != psoMap.end())
            pipelineState = pair->second.pipelineState;

        else
        {
            device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineState));

            PipelineStateCache cache;
            cache.pipelineState = pipelineState;
            cache.vertexShader = vertexShader;
            cache.pixelShader = pixelShader;
            cache.vertexDeclaration = vertexDeclaration;

            psoMap.insert(std::make_pair(hash, std::move(cache)));
        }

        commandList->SetPipelineState(pipelineState.Get());
    }

    dirtyState.reset();
}

void Device::updateDirty(void* dest, const void* src, const size_t byteSize, const DirtyStateIndex dirtyStateIndex)
{
    if (memcmp(dest, src, byteSize) == 0)
        return;

    dirtyState.set((size_t)dirtyStateIndex);
    memcpy(dest, src, byteSize);
}

void Device::submitAll()
{
    renderQueue.submitAll();

    // Reset everything
    uploadBufferPool.reset();
    viewPool.reset();
    samplerPool.reset();
    dirtyState.set();
}

Device::Device(D3DPRESENT_PARAMETERS* presentationParameters)
    : viewPool(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), samplerPool(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
{
#if _DEBUG
    ComPtr<ID3D12Debug> debugInterface;
    D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
    debugInterface->EnableDebugLayer();

    SetWindowLongPtr(presentationParameters->hDeviceWindow, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
    SetWindowPos(presentationParameters->hDeviceWindow, HWND_TOP, (1920 - 1600) / 2, (1080 - 900) / 2, 1600, 900, SWP_FRAMECHANGED);
#endif

    // Create DXGI factory
    ComPtr<IDXGIFactory4> dxgiFactory;
    CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

    // Get first DXGI adapter
    ComPtr<IDXGIAdapter> dxgiAdapter;
    dxgiFactory->EnumAdapters(0, dxgiAdapter.GetAddressOf());

    // Create device
    D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

    // Create allocator
    D3D12MA::ALLOCATOR_DESC allocatorDesc{};
    allocatorDesc.pDevice = device.Get();
    allocatorDesc.pAdapter = dxgiAdapter.Get();

    D3D12MA::CreateAllocator(&allocatorDesc, allocator.GetAddressOf());

    // Create command queues
    renderQueue.initialize(device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);
    loadQueue.initialize(device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);

    // Create swap chain
    ComPtr<IDXGISwapChain1> swapChain1;

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    swapChainDesc.Width = presentationParameters->BackBufferWidth;
    swapChainDesc.Height = presentationParameters->BackBufferHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    dxgiFactory->CreateSwapChainForHwnd(renderQueue.getCommandQueue(), presentationParameters->hDeviceWindow, &swapChainDesc, nullptr, nullptr, &swapChain1);
    swapChain1.As(&swapChain);

    // Initialize root signature
    CD3DX12_DESCRIPTOR_RANGE descriptorRanges[4];
    descriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0);
    descriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 1);
    descriptorRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, _countof(textures), 0, 2);
    descriptorRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, _countof(samplers), 0, 3);

    CD3DX12_ROOT_PARAMETER rootParameters[5];
    rootParameters[0].InitAsDescriptorTable(1, &descriptorRanges[0], D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[1].InitAsDescriptorTable(1, &descriptorRanges[1], D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[2].InitAsDescriptorTable(1, &descriptorRanges[2], D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[3].InitAsDescriptorTable(1, &descriptorRanges[3], D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[4].InitAsConstants(2, 0, 4, D3D12_SHADER_VISIBILITY_PIXEL);

    // Create root signature
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    
    ComPtr<ID3DBlob> rootSignatureBlob, errorBlob;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSignatureBlob, &errorBlob);

    if (errorBlob)
        OutputDebugStringA((LPCSTR)errorBlob->GetBufferPointer());

    device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

    // Initialize PSO
    pso.pRootSignature = rootSignature.Get();
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pso.SampleDesc = swapChainDesc.SampleDesc;
    pso.SampleMask = ~0;
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso.DepthStencilState.DepthEnable = true;
    pso.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    pso.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    pso.DSVFormat = DXGI_FORMAT_D32_FLOAT;

    // Init samplers
    for (auto& sampler : samplers)
    {
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
    }

    // Create swap chain render targets
    for (int i = 0; i < _countof(backBufferRenderTargets); i++)
    {
        ComPtr<ID3D12Resource> backBuffer;
        swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));

        backBufferRenderTargets[i] = new RenderTargetTexture(this, backBuffer.Get());
        backBufferRenderTargets[i]->setStates(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    }

    backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    // Transition from present to render target
    backBufferRenderTargets[backBufferIndex]->transition(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    // Invalidate pipeline state
    dirtyState.set();
}

ID3D12Device* Device::getDevice() const
{
    return device.Get();
}

D3D12MA::Allocator* Device::getAllocator() const
{
    return allocator.Get();
}

ID3D12GraphicsCommandList* Device::getCommandList() const
{
    return renderQueue.getCommandList();
}

CommandQueue& Device::getLoadQueue()
{
    return loadQueue;
}

HRESULT Device::createResource(D3D12_HEAP_TYPE HeapType, const D3D12_RESOURCE_DESC* pResourceDesc,
    D3D12_RESOURCE_STATES InitialResourceState, const D3D12_CLEAR_VALUE* pOptimizedClearValue,
    D3D12MA::Allocation** ppAllocation) const
{
    D3D12MA::ALLOCATION_DESC desc{};
    desc.HeapType = HeapType;

    return allocator->CreateResource(&desc, pResourceDesc, InitialResourceState, pOptimizedClearValue, ppAllocation,
                                     __uuidof(ID3D12Resource), nullptr);
}

FUNCTION_STUB(HRESULT, Device::TestCooperativeLevel)

FUNCTION_STUB(UINT, Device::GetAvailableTextureMem)

FUNCTION_STUB(HRESULT, Device::EvictManagedResources)

FUNCTION_STUB(HRESULT, Device::GetDirect3D, Direct3D9** ppD3D9)

FUNCTION_STUB(HRESULT, Device::GetDeviceCaps, D3DCAPS9* pCaps)

FUNCTION_STUB(HRESULT, Device::GetDisplayMode, UINT iSwapChain, D3DDISPLAYMODE* pMode)

FUNCTION_STUB(HRESULT, Device::GetCreationParameters, D3DDEVICE_CREATION_PARAMETERS *pParameters)

FUNCTION_STUB(HRESULT, Device::SetCursorProperties, UINT XHotSpot, UINT YHotSpot, Surface* pCursorBitmap)

FUNCTION_STUB(void, Device::SetCursorPosition, int X, int Y, DWORD Flags)

FUNCTION_STUB(BOOL, Device::ShowCursor, BOOL bShow)

FUNCTION_STUB(HRESULT, Device::CreateAdditionalSwapChain, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain)

FUNCTION_STUB(HRESULT, Device::GetSwapChain, UINT iSwapChain, IDirect3DSwapChain9** pSwapChain)

FUNCTION_STUB(UINT, Device::GetNumberOfSwapChains)

FUNCTION_STUB(HRESULT, Device::Reset, D3DPRESENT_PARAMETERS* pPresentationParameters)

HRESULT Device::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
    // Transition from render target to present
    backBufferRenderTargets[backBufferIndex]->transition(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    // Submit all
    submitAll();

    // Present
    swapChain->Present(1, 0);

#if 0
    WCHAR* stats;
    allocator->BuildStatsString(&stats, FALSE);
    printf("%ls\n", stats);
    allocator->FreeStatsString(stats);
#endif

    // Set render target index
    backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    // Transition from present to render target
    backBufferRenderTargets[backBufferIndex]->transition(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    return S_OK;
}

HRESULT Device::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, Surface** ppBackBuffer)
{
    backBufferRenderTargets[backBufferIndex]->GetSurfaceLevel(0, ppBackBuffer);
    return S_OK;
}
    
FUNCTION_STUB(HRESULT, Device::GetRasterStatus, UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)

FUNCTION_STUB(HRESULT, Device::SetDialogBoxMode, BOOL bEnableDialogs)

FUNCTION_STUB(void, Device::SetGammaRamp, UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)

FUNCTION_STUB(void, Device::GetGammaRamp, UINT iSwapChain, D3DGAMMARAMP* pRamp)

HRESULT Device::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, Texture** ppTexture, HANDLE* pSharedHandle)
{
    ComPtr<D3D12MA::Allocation> texture;

    const DXGI_FORMAT format = TypeConverter::makeUntypeless(TypeConverter::convert(Format), false);

    if (format == DXGI_FORMAT_UNKNOWN)
    {
        *ppTexture = nullptr;
        return E_FAIL;
    }

    const FLOAT zero[4] = { 0, 0, 0, 0 };

    const HRESULT hr = createResource
    (
        D3D12_HEAP_TYPE_DEFAULT,

        &CD3DX12_RESOURCE_DESC::Tex2D(format, Width, Height, 1, Levels, 1, 0,
            Usage & D3DUSAGE_RENDERTARGET ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : 
            Usage & D3DUSAGE_DEPTHSTENCIL ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE),

        Usage & D3DUSAGE_RENDERTARGET ? D3D12_RESOURCE_STATE_RENDER_TARGET :
        Usage & D3DUSAGE_DEPTHSTENCIL ? D3D12_RESOURCE_STATE_DEPTH_WRITE : D3D12_RESOURCE_STATE_COMMON,

        Usage & D3DUSAGE_RENDERTARGET ? &CD3DX12_CLEAR_VALUE(format, zero) :
        Usage & D3DUSAGE_DEPTHSTENCIL ? &CD3DX12_CLEAR_VALUE(format, 1.0f, 0) : nullptr,

        &texture
    );
        
    if (FAILED(hr))
        return hr;

    if (Usage & D3DUSAGE_RENDERTARGET)
    {
        *ppTexture = new RenderTargetTexture(this, texture.Get());
        (*ppTexture)->setStates(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    else if (Usage & D3DUSAGE_DEPTHSTENCIL)
    {
        *ppTexture = new DepthStencilTexture(this, texture.Get());
        (*ppTexture)->setStates(D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    else
        *ppTexture = new Texture(this, texture.Get());

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::CreateVolumeTexture, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)

FUNCTION_STUB(HRESULT, Device::CreateCubeTexture, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, CubeTexture** ppCubeTexture, HANDLE* pSharedHandle)

HRESULT Device::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, VertexBuffer** ppVertexBuffer, HANDLE* pSharedHandle)
{
    *ppVertexBuffer = new VertexBuffer(this, Length);
    return S_OK;
}

HRESULT Device::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IndexBuffer** ppIndexBuffer, HANDLE* pSharedHandle)
{
    *ppIndexBuffer = new IndexBuffer(this, Length, TypeConverter::convert(Format));
    return S_OK;
}

HRESULT Device::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, Surface** ppSurface, HANDLE* pSharedHandle)
{
    ComPtr<RenderTargetTexture> renderTargetTex;
    const HRESULT hr = CreateTexture(Width, Height, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, reinterpret_cast<Texture**>(renderTargetTex.GetAddressOf()), pSharedHandle);
    if (FAILED(hr))
        return hr;

    renderTargetTex->GetSurfaceLevel(0, ppSurface);
    return S_OK;
}

HRESULT Device::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, Surface** ppSurface, HANDLE* pSharedHandle)
{
    ComPtr<DepthStencilTexture> depthStencilTex;
    const HRESULT hr = CreateTexture(Width, Height, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, reinterpret_cast<Texture**>(depthStencilTex.GetAddressOf()), pSharedHandle);
    if (FAILED(hr))
        return hr;

    depthStencilTex->GetSurfaceLevel(0, ppSurface);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::UpdateSurface, Surface* pSourceSurface, CONST RECT* pSourceRect, Surface* pDestinationSurface, CONST POINT* pDestPoint)

FUNCTION_STUB(HRESULT, Device::UpdateTexture, BaseTexture* pSourceTexture, BaseTexture* pDestinationTexture)

FUNCTION_STUB(HRESULT, Device::GetRenderTargetData, Surface* pRenderTarget, Surface* pDestSurface)

FUNCTION_STUB(HRESULT, Device::GetFrontBufferData, UINT iSwapChain, Surface* pDestSurface)

HRESULT Device::StretchRect(Surface* pSourceSurface, CONST RECT* pSourceRect, Surface* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::ColorFill, Surface* pSurface, CONST RECT* pRect, D3DCOLOR color)

FUNCTION_STUB(HRESULT, Device::CreateOffscreenPlainSurface, UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, Surface** ppSurface, HANDLE* pSharedHandle)

HRESULT Device::SetRenderTarget(DWORD RenderTargetIndex, Surface* pRenderTarget)
{
    if (pRenderTarget)
    {
        const auto renderTargetTexture = ((RenderTargetSurface*)pRenderTarget)->getTexture();
        if (renderTargets[RenderTargetIndex].Get() != renderTargetTexture)
            dirtyState.set((size_t)DirtyStateIndex::RenderTarget);

        renderTargets[RenderTargetIndex] = renderTargetTexture;
        updateDirty(pso.RTVFormats[RenderTargetIndex], renderTargetTexture->getFormat(), DirtyStateIndex::PipelineState);
    }

    else
    {
        if (renderTargets[RenderTargetIndex] != nullptr)
            dirtyState.set((size_t)DirtyStateIndex::RenderTarget);

        renderTargets[RenderTargetIndex] = nullptr;
        updateDirty(pso.RTVFormats[RenderTargetIndex], DXGI_FORMAT_UNKNOWN, DirtyStateIndex::PipelineState);
    }

    return S_OK;
}

HRESULT Device::GetRenderTarget(DWORD RenderTargetIndex, Surface** ppRenderTarget)
{
    return S_OK;
}

HRESULT Device::SetDepthStencilSurface(Surface* pNewZStencil)
{
    if (pNewZStencil)
    {
        const auto depthStencilTexture = ((DepthStencilSurface*)pNewZStencil)->getTexture();

        if (depthStencil.Get() != depthStencilTexture)
            dirtyState.set((size_t)DirtyStateIndex::RenderTarget);

        depthStencil = depthStencilTexture;
        updateDirty(pso.DSVFormat, depthStencilTexture->getFormat(), DirtyStateIndex::PipelineState);
    }

    else
    {
        if (depthStencil != nullptr)
            dirtyState.set((size_t)DirtyStateIndex::RenderTarget);

        depthStencil = nullptr;
        updateDirty(pso.DSVFormat, DXGI_FORMAT_UNKNOWN, DirtyStateIndex::PipelineState);
    }

    return S_OK;
}

HRESULT Device::GetDepthStencilSurface(Surface** ppZStencilSurface)
{
    if (depthStencil)
        depthStencil->GetSurfaceLevel(0, ppZStencilSurface);
    else
        *ppZStencilSurface = nullptr;

    return S_OK;
}

HRESULT Device::BeginScene()
{
    return S_OK;
}

HRESULT Device::EndScene()
{
    return S_OK;
}

HRESULT Device::Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
    ID3D12GraphicsCommandList* commandList = renderQueue.getCommandList();

    if (Flags & D3DCLEAR_TARGET)
    {
        FLOAT color[4];
        color[2] = (Color & 0xFF) / 255.0f;
        color[1] = ((Color >> 8) & 0xFF) / 255.0f;
        color[0] = ((Color >> 16) & 0xFF) / 255.0f;
        color[3] = ((Color >> 24) & 0xFF) / 255.0f;

        for (auto& renderTarget : renderTargets)
        {
            if (renderTarget)
            {
                renderTarget->transition(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
                commandList->ClearRenderTargetView(renderTarget->getRtvDescriptorHandle(), color, 0, nullptr);
            }
        }
    }

    if (Flags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL) && depthStencil)
    {
        D3D12_CLEAR_FLAGS clearFlags = (D3D12_CLEAR_FLAGS)0;

        if (Flags & D3DCLEAR_ZBUFFER)
            clearFlags |= D3D12_CLEAR_FLAG_DEPTH;

        if (Flags & D3DCLEAR_STENCIL)
            clearFlags |= D3D12_CLEAR_FLAG_STENCIL;

        depthStencil->transition(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        commandList->ClearDepthStencilView(depthStencil->getDsvDescriptorHandle(), clearFlags, Z, (UINT8)Stencil, 0, nullptr);
    }

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::SetTransform, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)

FUNCTION_STUB(HRESULT, Device::GetTransform, D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)

FUNCTION_STUB(HRESULT, Device::MultiplyTransform, D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*)

HRESULT Device::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
    D3D12_VIEWPORT newViewport;
    newViewport.TopLeftX = (float)pViewport->X;
    newViewport.TopLeftY = (float)pViewport->Y;
    newViewport.Width = (float)pViewport->Width;
    newViewport.Height = (float)pViewport->Height;
    newViewport.MinDepth = pViewport->MinZ;
    newViewport.MaxDepth = pViewport->MaxZ;

    updateDirty(&viewport, &newViewport, sizeof(D3D12_VIEWPORT), DirtyStateIndex::Viewport);

    return S_OK;
}

HRESULT Device::GetViewport(D3DVIEWPORT9* pViewport)
{
    pViewport->X = (DWORD)viewport.TopLeftX;
    pViewport->Y = (DWORD)viewport.TopLeftY;
    pViewport->Width = (DWORD)viewport.Width;
    pViewport->Height = (DWORD)viewport.Height;
    pViewport->MinZ = viewport.MinDepth;
    pViewport->MaxZ = viewport.MaxDepth;
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::SetMaterial, CONST D3DMATERIAL9* pMaterial)

FUNCTION_STUB(HRESULT, Device::GetMaterial, D3DMATERIAL9* pMaterial)

FUNCTION_STUB(HRESULT, Device::SetLight, DWORD Index, CONST D3DLIGHT9*)

FUNCTION_STUB(HRESULT, Device::GetLight, DWORD Index, D3DLIGHT9*)

FUNCTION_STUB(HRESULT, Device::LightEnable, DWORD Index, BOOL Enable)

FUNCTION_STUB(HRESULT, Device::GetLightEnable, DWORD Index, BOOL* pEnable)

FUNCTION_STUB(HRESULT, Device::SetClipPlane, DWORD Index, CONST float* pPlane)

FUNCTION_STUB(HRESULT, Device::GetClipPlane, DWORD Index, float* pPlane)

HRESULT Device::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
    switch (State)
    {
    case D3DRS_ZENABLE:
        updateDirty(pso.DepthStencilState.DepthEnable, (BOOL)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_FILLMODE:
        updateDirty(pso.RasterizerState.FillMode, (D3D12_FILL_MODE)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_ZWRITEENABLE:
        updateDirty(pso.DepthStencilState.DepthWriteMask, (D3D12_DEPTH_WRITE_MASK)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_ALPHATESTENABLE:
        updateDirty(alphaTestEnable, (BOOL)Value, DirtyStateIndex::AlphaTest);
        break;

    case D3DRS_SRCBLEND:
        updateDirty(pso.BlendState.RenderTarget[0].SrcBlend, (D3D12_BLEND)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_DESTBLEND:
        updateDirty(pso.BlendState.RenderTarget[0].DestBlend, (D3D12_BLEND)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_CULLMODE:
        updateDirty(pso.RasterizerState.CullMode, (D3D12_CULL_MODE)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_ZFUNC:
        updateDirty(pso.DepthStencilState.DepthFunc, (D3D12_COMPARISON_FUNC)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_ALPHAREF:
        updateDirty(alphaRef, (float)Value / 255.0f, DirtyStateIndex::AlphaTest);
        break;

    case D3DRS_ALPHABLENDENABLE:
        updateDirty(pso.BlendState.RenderTarget[0].BlendEnable, (BOOL)Value, DirtyStateIndex::PipelineState);
        break;
        /*
    case D3DRS_STENCILENABLE:
        updateMemory(pso.DepthStencilState.StencilEnable, (BOOL)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_STENCILFAIL:
        updateMemory(pso.DepthStencilState.FrontFace.StencilFailOp, (D3D12_STENCIL_OP)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_STENCILZFAIL:
        updateMemory(pso.DepthStencilState.FrontFace.StencilDepthFailOp, (D3D12_STENCIL_OP)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_STENCILPASS:
        updateMemory(pso.DepthStencilState.FrontFace.StencilPassOp, (D3D12_STENCIL_OP)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_STENCILFUNC:
        updateMemory(pso.DepthStencilState.FrontFace.StencilFunc, (D3D12_COMPARISON_FUNC)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_STENCILREF:
        stencilRef, (UINT)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_STENCILMASK:
        updateMemory(pso.DepthStencilState.StencilReadMask, (UINT8)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_STENCILWRITEMASK:
        updateMemory(pso.DepthStencilState.StencilWriteMask, (UINT8)Value, DirtyStateIndex::PipelineState);
        break;
        */
    case D3DRS_COLORWRITEENABLE:
        updateDirty(pso.BlendState.RenderTarget[0].RenderTargetWriteMask, (UINT8)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_BLENDOP:
        updateDirty(pso.BlendState.RenderTarget[0].BlendOp, (D3D12_BLEND_OP)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_SLOPESCALEDEPTHBIAS:
        updateDirty(pso.RasterizerState.SlopeScaledDepthBias, *(float*)&Value, DirtyStateIndex::PipelineState);
        break;
        /*/
    case D3DRS_CCW_STENCILFAIL:
        updateMemory(pso.DepthStencilState.BackFace.StencilFailOp, (D3D12_STENCIL_OP)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_CCW_STENCILZFAIL:
        updateMemory(pso.DepthStencilState.BackFace.StencilDepthFailOp, (D3D12_STENCIL_OP)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_CCW_STENCILPASS:
        updateMemory(pso.DepthStencilState.BackFace.StencilPassOp, (D3D12_STENCIL_OP)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_CCW_STENCILFUNC:
        updateMemory(pso.DepthStencilState.BackFace.StencilFunc, (D3D12_COMPARISON_FUNC)Value, DirtyStateIndex::PipelineState);
        break;
        */
    case D3DRS_COLORWRITEENABLE1:
        updateDirty(pso.BlendState.RenderTarget[1].RenderTargetWriteMask, (UINT8)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_COLORWRITEENABLE2:
        updateDirty(pso.BlendState.RenderTarget[2].RenderTargetWriteMask, (UINT8)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_COLORWRITEENABLE3: 
        updateDirty(pso.BlendState.RenderTarget[3].RenderTargetWriteMask, (UINT8)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_DEPTHBIAS:
        updateDirty(pso.RasterizerState.DepthBias, (INT)(*(float*)&Value * (1 << 24)), DirtyStateIndex::PipelineState);
        break;

    case D3DRS_SRCBLENDALPHA:
        updateDirty(pso.BlendState.RenderTarget[0].SrcBlendAlpha, (D3D12_BLEND)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_DESTBLENDALPHA:
        updateDirty(pso.BlendState.RenderTarget[0].DestBlendAlpha, (D3D12_BLEND)Value, DirtyStateIndex::PipelineState);
        break;

    case D3DRS_BLENDOPALPHA:
        updateDirty(pso.BlendState.RenderTarget[0].BlendOpAlpha, (D3D12_BLEND_OP)Value, DirtyStateIndex::PipelineState);
        break;
    }

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetRenderState, D3DRENDERSTATETYPE State, DWORD* pValue)

FUNCTION_STUB(HRESULT, Device::CreateStateBlock, D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)

FUNCTION_STUB(HRESULT, Device::BeginStateBlock)

FUNCTION_STUB(HRESULT, Device::EndStateBlock, IDirect3DStateBlock9** ppSB)

FUNCTION_STUB(HRESULT, Device::SetClipStatus, CONST D3DCLIPSTATUS9* pClipStatus)

FUNCTION_STUB(HRESULT, Device::GetClipStatus, D3DCLIPSTATUS9* pClipStatus)

FUNCTION_STUB(HRESULT, Device::GetTexture, DWORD Stage, BaseTexture** ppTexture)

HRESULT Device::SetTexture(DWORD Stage, BaseTexture* pTexture)
{
    if (textures[Stage].Get() != pTexture)
        dirtyState.set((size_t)DirtyStateIndex::Texture);

    textures[Stage] = reinterpret_cast<Texture*>(pTexture);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetTextureStageState, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)

HRESULT Device::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetSamplerState, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)

HRESULT Device::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
    switch (Type)
    {
    case D3DSAMP_ADDRESSU:
        updateDirty(samplers[Sampler].AddressU, (D3D12_TEXTURE_ADDRESS_MODE)Value, DirtyStateIndex::Sampler);
        break;

    case D3DSAMP_ADDRESSV:
        updateDirty(samplers[Sampler].AddressV, (D3D12_TEXTURE_ADDRESS_MODE)Value, DirtyStateIndex::Sampler);
        break;

    case D3DSAMP_ADDRESSW:
        updateDirty(samplers[Sampler].AddressW, (D3D12_TEXTURE_ADDRESS_MODE)Value, DirtyStateIndex::Sampler);
        break;

    case D3DSAMP_BORDERCOLOR:
        FLOAT color[4];
        color[2] = (Value & 0xFF) / 255.0f;
        color[1] = ((Value >> 8) & 0xFF) / 255.0f;
        color[0] = ((Value >> 16) & 0xFF) / 255.0f;
        color[3] = ((Value >> 24) & 0xFF) / 255.0f;

        updateDirty(samplers[Sampler].BorderColor, color, sizeof(samplers[Sampler].BorderColor), DirtyStateIndex::Sampler);
        break;

    case D3DSAMP_MAGFILTER:
        if (Value == D3DTEXF_LINEAR)
            updateDirty(samplers[Sampler].Filter, (D3D12_FILTER)(samplers[Sampler].Filter | 0x4), DirtyStateIndex::Sampler);
        else
            updateDirty(samplers[Sampler].Filter, (D3D12_FILTER)(samplers[Sampler].Filter & ~0x4), DirtyStateIndex::Sampler);

        break;

    case D3DSAMP_MINFILTER:
        if (Value == D3DTEXF_LINEAR)
            updateDirty(samplers[Sampler].Filter, (D3D12_FILTER)(samplers[Sampler].Filter | 0x10), DirtyStateIndex::Sampler);
        else
            updateDirty(samplers[Sampler].Filter, (D3D12_FILTER)(samplers[Sampler].Filter & ~0x10), DirtyStateIndex::Sampler);

        break;

    case D3DSAMP_MIPFILTER:
        if (Value == D3DTEXF_LINEAR)
            updateDirty(samplers[Sampler].Filter, (D3D12_FILTER)(samplers[Sampler].Filter | 0x1), DirtyStateIndex::Sampler);
        else
            updateDirty(samplers[Sampler].Filter, (D3D12_FILTER)(samplers[Sampler].Filter & ~0x1), DirtyStateIndex::Sampler);

        break;
        
    }
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::ValidateDevice, DWORD* pNumPasses)

FUNCTION_STUB(HRESULT, Device::SetPaletteEntries, UINT PaletteNumber, CONST PALETTEENTRY* pEntries)

FUNCTION_STUB(HRESULT, Device::GetPaletteEntries, UINT PaletteNumber, PALETTEENTRY* pEntries)

FUNCTION_STUB(HRESULT, Device::SetCurrentTexturePalette, UINT PaletteNumber)

FUNCTION_STUB(HRESULT, Device::GetCurrentTexturePalette, UINT *PaletteNumber)

HRESULT Device::SetScissorRect(CONST RECT* pRect)
{
    D3D12_RECT newScissorRect;
    newScissorRect.left = pRect->left;
    newScissorRect.top = pRect->top;
    newScissorRect.right = pRect->right;
    newScissorRect.bottom = pRect->bottom;

    updateDirty(&scissorRect, &newScissorRect, sizeof(D3D12_RECT), DirtyStateIndex::ScissorRect);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetScissorRect, RECT* pRect)

FUNCTION_STUB(HRESULT, Device::SetSoftwareVertexProcessing, BOOL bSoftware)

FUNCTION_STUB(BOOL, Device::GetSoftwareVertexProcessing)

FUNCTION_STUB(HRESULT, Device::SetNPatchMode, float nSegments)

FUNCTION_STUB(float, Device::GetNPatchMode)

HRESULT Device::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
    if (PrimitiveType > D3DPT_TRIANGLESTRIP)
        return S_OK;

    updateDirty(primitiveTopology, (D3D_PRIMITIVE_TOPOLOGY)PrimitiveType, DirtyStateIndex::PrimitiveTopology);
    updatePipelineState();
    renderQueue.getCommandList()->DrawInstanced(calculateIndexCount(PrimitiveType, PrimitiveCount), 1, StartVertex, 0);

    return S_OK;
}       
        
HRESULT Device::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
    if (PrimitiveType > D3DPT_TRIANGLESTRIP)
        return S_OK;

    updateDirty(primitiveTopology, (D3D_PRIMITIVE_TOPOLOGY)PrimitiveType, DirtyStateIndex::PrimitiveTopology);
    updatePipelineState();
    renderQueue.getCommandList()->DrawIndexedInstanced(calculateIndexCount(PrimitiveType, primCount), 1, startIndex, BaseVertexIndex, 0);

    return S_OK;
}       
        
HRESULT Device::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    if (PrimitiveType > D3DPT_TRIANGLESTRIP)
        return S_OK;

    const UINT vertexCount = calculateIndexCount(PrimitiveType, PrimitiveCount); // index count == vertex count

    // Calculate total byte size
    const UINT vertexSize = vertexCount * VertexStreamZeroStride;

    // Allocate vertex buffer
    const UploadBuffer vertexBuffer = uploadBufferPool.allocate(allocator.Get(), vertexSize);

    // Copy to vertex buffer
    memcpy(vertexBuffer.data, pVertexStreamZeroData, vertexSize);

    // Initialize first vertex buffer view
    vertexBufferViews[0].BufferLocation = vertexBuffer.gpuVirtualAddress;
    vertexBufferViews[0].SizeInBytes = vertexSize;
    vertexBufferViews[0].StrideInBytes = VertexStreamZeroStride;

    // Notify that the vertex buffer is dirty
    dirtyState.set((size_t)DirtyStateIndex::VertexBuffer);

    // Draw
    updateDirty(primitiveTopology, (D3D_PRIMITIVE_TOPOLOGY)PrimitiveType, DirtyStateIndex::PrimitiveTopology);
    updatePipelineState();
    renderQueue.getCommandList()->DrawInstanced(vertexCount, 1, 0, 0);

    return S_OK;
}       
        
FUNCTION_STUB(HRESULT, Device::DrawIndexedPrimitiveUP, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)

FUNCTION_STUB(HRESULT, Device::ProcessVertices, UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, VertexBuffer* pDestBuffer, VertexDeclaration* pVertexDecl, DWORD Flags)

HRESULT Device::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, VertexDeclaration** ppDecl)
{
    *ppDecl = new VertexDeclaration(pVertexElements);
    return S_OK;
}

HRESULT Device::SetVertexDeclaration(VertexDeclaration* pDecl)
{
    if (vertexDeclaration.Get() != pDecl)
        dirtyState.set((size_t)DirtyStateIndex::VertexDeclaration);

    vertexDeclaration = pDecl;
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexDeclaration, VertexDeclaration** ppDecl)

HRESULT Device::SetFVF(DWORD FVF)
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetFVF, DWORD* pFVF)

HRESULT Device::CreateVertexShader(CONST DWORD* pFunction, VertexShader** ppShader, DWORD FunctionSize)
{
    *ppShader = new VertexShader(pFunction, FunctionSize);
    return S_OK;
}

HRESULT Device::SetVertexShader(VertexShader* pShader)
{
    if (vertexShader.Get() != pShader)
        dirtyState.set((size_t)DirtyStateIndex::VertexShader);

    vertexShader = pShader;
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShader, VertexShader** ppShader)

HRESULT Device::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    updateDirty(&vertexConstants.c[StartRegister], pConstantData, Vector4fCount * sizeof(FLOAT[4]), DirtyStateIndex::VertexConstant);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantF, UINT StartRegister, float* pConstantData, UINT Vector4fCount)

HRESULT Device::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    updateDirty(&vertexConstants.i[StartRegister], pConstantData, Vector4iCount * sizeof(INT[4]), DirtyStateIndex::VertexConstant);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantI, UINT StartRegister, int* pConstantData, UINT Vector4iCount)

HRESULT Device::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
    updateDirty(&vertexConstants.b[StartRegister], pConstantData, BoolCount * sizeof(BOOL), DirtyStateIndex::VertexConstant);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantB, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)

HRESULT Device::SetStreamSource(UINT StreamNumber, VertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride)
{
    if (vertexBuffers[StreamNumber].Get() != pStreamData)
        dirtyState.set((size_t)DirtyStateIndex::VertexBuffer);

    vertexBuffers[StreamNumber] = pStreamData;

    updateDirty(vertexBufferViews[StreamNumber],
                pStreamData ? pStreamData->getVertexBufferView(OffsetInBytes, Stride) : D3D12_VERTEX_BUFFER_VIEW(),
                DirtyStateIndex::VertexBuffer);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetStreamSource, UINT StreamNumber, VertexBuffer** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)

HRESULT Device::SetStreamSourceFreq(UINT StreamNumber, UINT Setting)
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetStreamSourceFreq, UINT StreamNumber, UINT* pSetting)

HRESULT Device::SetIndices(IndexBuffer* pIndexData)
{
    if (indexBuffer.Get() != pIndexData)
        dirtyState.set((size_t)DirtyStateIndex::IndexBuffer);

    indexBuffer = pIndexData;
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetIndices, IndexBuffer** ppIndexData)

HRESULT Device::CreatePixelShader(CONST DWORD* pFunction, PixelShader** ppShader, DWORD FunctionSize)
{
    *ppShader = new PixelShader(pFunction, FunctionSize);
    return S_OK;
}

HRESULT Device::SetPixelShader(PixelShader* pShader)
{
    if (pixelShader.Get() != pShader)
        dirtyState.set((size_t)DirtyStateIndex::PixelShader);

    pixelShader = pShader;
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShader, PixelShader** ppShader)

HRESULT Device::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    updateDirty(&pixelConstants.c[StartRegister], pConstantData, Vector4fCount * sizeof(FLOAT[4]), DirtyStateIndex::PixelConstant);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantF, UINT StartRegister, float* pConstantData, UINT Vector4fCount)

HRESULT Device::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    updateDirty(&pixelConstants.i[StartRegister], pConstantData, Vector4iCount * sizeof(INT[4]), DirtyStateIndex::PixelConstant);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantI, UINT StartRegister, int* pConstantData, UINT Vector4iCount)

HRESULT Device::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
    updateDirty(&pixelConstants.b[StartRegister], pConstantData, BoolCount * sizeof(BOOL), DirtyStateIndex::PixelConstant);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantB, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)

FUNCTION_STUB(HRESULT, Device::DrawRectPatch, UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)

FUNCTION_STUB(HRESULT, Device::DrawTriPatch, UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)

FUNCTION_STUB(HRESULT, Device::DeletePatch, UINT Handle)

FUNCTION_STUB(HRESULT, Device::CreateQuery, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)