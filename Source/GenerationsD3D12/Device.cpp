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

struct VertexConstants
{
    FLOAT c[256][4];
    INT i[16][4];
    BOOL b[16];
};

struct PixelConstants
{
    FLOAT c[224][4];
    INT i[16][4];
    BOOL b[16];
};

void Device::prepareDraw()
{
    if (renderTargets[0] == backBufferRenderTargets[!backBufferIndex])
        renderTargets[0] = backBufferRenderTargets[backBufferIndex];

    size_t renderTargetCount;
    for (renderTargetCount = 0; renderTargetCount < _countof(renderTargets) && renderTargets[renderTargetCount]; renderTargetCount++)
        ;

    D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDescriptorHandles[_countof(renderTargets)];
    for (size_t i = 0; i < renderTargetCount; i++)
        renderTargetDescriptorHandles[i] = renderTargets[i]->getRtvDescriptorHandle();

    pso.NumRenderTargets = renderTargetCount;

    size_t vertexBufferCount;
    for (vertexBufferCount = 0; vertexBufferCount < _countof(vertexBufferViews) && vertexBufferViews[vertexBufferCount].BufferLocation; vertexBufferCount++)
        ;

    const size_t srvIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    const size_t samplerIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    for (size_t i = 0; i < _countof(textures); i++)
    {
        if (!textures[i])
            continue;

        device->CopyDescriptorsSimple(1, CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuDescriptorHandle, i, srvIncrementSize),
                                      textures[i]->getSrvDescriptorHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    for (size_t i = 0; i < _countof(samplers); i++)
        device->CreateSampler(&samplers[i], CD3DX12_CPU_DESCRIPTOR_HANDLE(samplerCpuDescriptorHandle, i, samplerIncrementSize));

    if (vertexShader)
        pso.VS = vertexShader->getShaderByteCode();

    if (pixelShader)
        pso.PS = pixelShader->getShaderByteCode();

    if (vertexDeclaration)
        pso.InputLayout = vertexDeclaration->getInputLayoutDesc();

    ID3D12GraphicsCommandList* commandList = renderQueue.getCommandList();

    commandList->OMSetRenderTargets(renderTargetCount, renderTargetDescriptorHandles, FALSE, depthStencil ? &depthStencil->getDsvDescriptorHandle() : nullptr);
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);
    commandList->IASetIndexBuffer(&indexBufferView);
    commandList->IASetVertexBuffers(0, vertexBufferCount, vertexBufferViews);

    commandList->SetGraphicsRootSignature(rootSignature.Get());

    ID3D12DescriptorHeap* vertexConstantsDescriptorHeap = vertexConstants.getDescriptorHeap();
    ID3D12DescriptorHeap* pixelConstantsDescriptorHeap = pixelConstants.getDescriptorHeap();

    commandList->SetDescriptorHeaps(1, &vertexConstantsDescriptorHeap);
    commandList->SetGraphicsRootDescriptorTable(0, vertexConstants.getGpuDescriptorHandle());

    commandList->SetDescriptorHeaps(1, &pixelConstantsDescriptorHeap);
    commandList->SetGraphicsRootDescriptorTable(1, pixelConstants.getGpuDescriptorHandle());

    commandList->SetDescriptorHeaps(1, srvDescriptorHeap.GetAddressOf());
    commandList->SetGraphicsRootDescriptorTable(2, srvGpuDescriptorHandle);

    commandList->SetDescriptorHeaps(1, samplerDescriptorHeap.GetAddressOf());
    commandList->SetGraphicsRootDescriptorTable(3, samplerGpuDescriptorHandle);

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

void Device::requestDraw()
{
    if (pendingDraw)
        return;

    prepareDraw();
    pendingDraw = true;
}

void Device::submitDraw()
{
    if (!pendingDraw)
        return;

    renderQueue.submitAll();
    pendingDraw = false;
}

void Device::updateMemoryAndNotify(void* dest, const void* src, const size_t byteSize)
{
    if (memcmp(dest, src, byteSize) == 0)
        return;

    submitDraw();
    memcpy(dest, src, byteSize);
}

void Device::reserveVertexBuffer(const size_t length)
{
    if (length < vertexUploadBufferSize)
        return;

    submitDraw();

    vertexUploadBufferSize = length;

    if (vertexUploadBuffer)
        vertexUploadBuffer->Unmap(0, nullptr);

    device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
                                    &CD3DX12_RESOURCE_DESC::Buffer(vertexUploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ,
                                    nullptr, IID_PPV_ARGS(&vertexUploadBuffer));

    vertexUploadBuffer->Map(0, &CD3DX12_RANGE(0, 0), &vertexUploadBufferData);
}

void Device::reserveIndexBuffer(const size_t length)
{
    if (length < indexUploadBufferSize)
        return;

    submitDraw();

    indexUploadBufferSize = length;

    if (indexUploadBuffer)
        indexUploadBuffer->Unmap(0, nullptr);

    device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
                                    &CD3DX12_RESOURCE_DESC::Buffer(indexUploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ,
                                    nullptr, IID_PPV_ARGS(&indexUploadBuffer));

    assert(indexUploadBuffer != nullptr);

    indexUploadBuffer->Map(0, &CD3DX12_RANGE(0, 0), &indexUploadBufferData);
}

Device::Device(D3DPRESENT_PARAMETERS* presentationParameters)
{
#if _DEBUG
    ComPtr<ID3D12Debug> debugInterface;
    D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
    debugInterface->EnableDebugLayer();

    SetWindowLongPtr(presentationParameters->hDeviceWindow, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
    SetWindowPos(presentationParameters->hDeviceWindow, HWND_TOP, (1920 - 1600) / 2, (1080 - 900) / 2, 1600, 900, SWP_FRAMECHANGED);
#endif

    // Create device
    D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

    // Create DXGI factory
    ComPtr<IDXGIFactory4> dxgiFactory;
    CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

    // Create command queues
    renderQueue.initialize(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
    loadQueue.initialize(device, D3D12_COMMAND_LIST_TYPE_DIRECT);

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

    CD3DX12_ROOT_PARAMETER rootParameters[4];
    rootParameters[0].InitAsDescriptorTable(1, &descriptorRanges[0], D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[1].InitAsDescriptorTable(1, &descriptorRanges[1], D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[2].InitAsDescriptorTable(1, &descriptorRanges[2], D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[3].InitAsDescriptorTable(1, &descriptorRanges[3], D3D12_SHADER_VISIBILITY_PIXEL);

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

    // Initialize constant buffers
    vertexConstants.initialize(device);
    pixelConstants.initialize(device);

    // Create SRV descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC srvDesc{};
    srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvDesc.NumDescriptors = _countof(textures);
    srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    device->CreateDescriptorHeap(&srvDesc, IID_PPV_ARGS(&srvDescriptorHeap));
    srvCpuDescriptorHandle = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    srvGpuDescriptorHandle = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

    // Create sampler descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC samplerDesc{};
    samplerDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    samplerDesc.NumDescriptors = _countof(samplers);
    samplerDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    device->CreateDescriptorHeap(&samplerDesc, IID_PPV_ARGS(&samplerDescriptorHeap));
    samplerCpuDescriptorHandle = samplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    samplerGpuDescriptorHandle = samplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

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
        backBufferRenderTargets[i] = new RenderTargetTexture(this, backBuffer);
    }

    backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    // Transition from present to render target
    renderQueue.getCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
                                                       backBufferRenderTargets[backBufferIndex]->getResource(),
                                                       D3D12_RESOURCE_STATE_PRESENT,
                                                       D3D12_RESOURCE_STATE_RENDER_TARGET));
}

ID3D12Device* Device::getUnderlyingDevice() const
{
    return device.Get();
}

CommandQueue& Device::getLoadQueue()
{
    return loadQueue;
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
    renderQueue.getCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
                                                       backBufferRenderTargets[backBufferIndex]->getResource(),
                                                       D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                       D3D12_RESOURCE_STATE_PRESENT));

    // Submit all
    if (pendingDraw)
        submitDraw();
    else
        renderQueue.submitAll();

    // Present
    swapChain->Present(1, 0);

    // Set render target index
    backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    // Transition from present to render target
    renderQueue.getCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
                                                       backBufferRenderTargets[backBufferIndex]->getResource(),
                                                       D3D12_RESOURCE_STATE_PRESENT,
                                                       D3D12_RESOURCE_STATE_RENDER_TARGET));
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
    ComPtr<ID3D12Resource> texture;

    const DXGI_FORMAT format = TypeConverter::makeUntypeless(TypeConverter::convert(Format), false);

    if (format == DXGI_FORMAT_UNKNOWN)
    {
        *ppTexture = nullptr;
        return E_FAIL;
    }

    const FLOAT black[4] = { 0, 0, 0, 1 };

    const HRESULT hr = device->CreateCommittedResource
    (
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),

        D3D12_HEAP_FLAG_NONE,

        &CD3DX12_RESOURCE_DESC::Tex2D(format, Width, Height, 1, Levels, 1, 0,
            Usage & D3DUSAGE_RENDERTARGET ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : 
            Usage & D3DUSAGE_DEPTHSTENCIL ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE),

        Usage & D3DUSAGE_RENDERTARGET ? D3D12_RESOURCE_STATE_RENDER_TARGET :
        Usage & D3DUSAGE_DEPTHSTENCIL ? D3D12_RESOURCE_STATE_DEPTH_WRITE : D3D12_RESOURCE_STATE_COMMON,

        Usage & D3DUSAGE_RENDERTARGET ? &CD3DX12_CLEAR_VALUE(format, black) :
        Usage & D3DUSAGE_DEPTHSTENCIL ? &CD3DX12_CLEAR_VALUE(format, 1.0f, 0) : nullptr,

        IID_PPV_ARGS(&texture)
    );
        
    if (FAILED(hr))
        return hr;

    if (Usage & D3DUSAGE_RENDERTARGET)
        *ppTexture = new RenderTargetTexture(this, texture);

    else if (Usage & D3DUSAGE_DEPTHSTENCIL)
        *ppTexture = new DepthStencilTexture(this, texture);

    else
        *ppTexture = new Texture(this, texture);

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
            submitDraw();

        renderTargets[RenderTargetIndex] = renderTargetTexture;
        pso.RTVFormats[RenderTargetIndex] = renderTargetTexture->getFormat();
    }

    else
    {
        if (renderTargets[RenderTargetIndex] != nullptr)
            submitDraw();

        renderTargets[RenderTargetIndex] = nullptr;
        pso.RTVFormats[RenderTargetIndex] = DXGI_FORMAT_UNKNOWN;
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
            submitDraw();

        depthStencil = depthStencilTexture;
        pso.DSVFormat = depthStencilTexture->getFormat();
    }

    else
    {
        if (depthStencil != nullptr)
            submitDraw();

        depthStencil = nullptr;
        pso.DSVFormat = DXGI_FORMAT_UNKNOWN;
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
        // Convert D3DCOLOR to FLOAT[4]
        FLOAT color[4];
        color[0] = ((Color >> 16) & 0xFF) / 255.0f;
        color[1] = ((Color >> 8) & 0xFF) / 255.0f;
        color[2] = (Color & 0xFF) / 255.0f;
        color[3] = ((Color >> 24) & 0xFF) / 255.0f;

        for (auto& renderTarget : renderTargets)
        {
            if (renderTarget)
                commandList->ClearRenderTargetView(renderTarget->getRtvDescriptorHandle(), color, 0, nullptr);
        }
    }

    if (Flags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL) && depthStencil)
    {
        D3D12_CLEAR_FLAGS clearFlags = (D3D12_CLEAR_FLAGS)0;

        if (Flags & D3DCLEAR_ZBUFFER)
            clearFlags |= D3D12_CLEAR_FLAG_DEPTH;

        if (Flags & D3DCLEAR_STENCIL)
            clearFlags |= D3D12_CLEAR_FLAG_STENCIL;

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

    updateMemoryAndNotify(&viewport, &newViewport, sizeof(D3D12_VIEWPORT));

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
        updateMemoryAndNotify(pso.DepthStencilState.DepthEnable, (BOOL)Value);
        break;

    case D3DRS_FILLMODE:
        updateMemoryAndNotify(pso.RasterizerState.FillMode, (D3D12_FILL_MODE)Value);
        break;

    case D3DRS_ZWRITEENABLE:
        updateMemoryAndNotify(pso.DepthStencilState.DepthWriteMask, (D3D12_DEPTH_WRITE_MASK)Value);
        break;

    case D3DRS_SRCBLEND:
        updateMemoryAndNotify(pso.BlendState.RenderTarget[0].SrcBlend, (D3D12_BLEND)Value);
        break;

    case D3DRS_DESTBLEND:
        updateMemoryAndNotify(pso.BlendState.RenderTarget[0].DestBlend, (D3D12_BLEND)Value);
        break;

    case D3DRS_CULLMODE:
        updateMemoryAndNotify(pso.RasterizerState.CullMode, (D3D12_CULL_MODE)Value);
        break;

    case D3DRS_ZFUNC:
        updateMemoryAndNotify(pso.DepthStencilState.DepthFunc, (D3D12_COMPARISON_FUNC)Value);
        break;

    case D3DRS_ALPHABLENDENABLE:
        updateMemoryAndNotify(pso.BlendState.RenderTarget[0].BlendEnable, (BOOL)Value);
        break;
        /*
    case D3DRS_STENCILENABLE:
        updateMemory(pso.DepthStencilState.StencilEnable, (BOOL)Value);
        break;

    case D3DRS_STENCILFAIL:
        updateMemory(pso.DepthStencilState.FrontFace.StencilFailOp, (D3D12_STENCIL_OP)Value);
        break;

    case D3DRS_STENCILZFAIL:
        updateMemory(pso.DepthStencilState.FrontFace.StencilDepthFailOp, (D3D12_STENCIL_OP)Value);
        break;

    case D3DRS_STENCILPASS:
        updateMemory(pso.DepthStencilState.FrontFace.StencilPassOp, (D3D12_STENCIL_OP)Value);
        break;

    case D3DRS_STENCILFUNC:
        updateMemory(pso.DepthStencilState.FrontFace.StencilFunc, (D3D12_COMPARISON_FUNC)Value);
        break;

    case D3DRS_STENCILREF:
        stencilRef, (UINT)Value);
        break;

    case D3DRS_STENCILMASK:
        updateMemory(pso.DepthStencilState.StencilReadMask, (UINT8)Value);
        break;

    case D3DRS_STENCILWRITEMASK:
        updateMemory(pso.DepthStencilState.StencilWriteMask, (UINT8)Value);
        break;
        */
    case D3DRS_COLORWRITEENABLE:
        updateMemoryAndNotify(pso.BlendState.RenderTarget[0].RenderTargetWriteMask, (UINT8)Value);
        break;

    case D3DRS_BLENDOP:
        updateMemoryAndNotify(pso.BlendState.RenderTarget[0].BlendOp, (D3D12_BLEND_OP)Value);
        break;

    case D3DRS_SLOPESCALEDEPTHBIAS:
        updateMemoryAndNotify(pso.RasterizerState.SlopeScaledDepthBias, *(float*)&Value);
        break;
        /*/
    case D3DRS_CCW_STENCILFAIL:
        updateMemory(pso.DepthStencilState.BackFace.StencilFailOp, (D3D12_STENCIL_OP)Value);
        break;

    case D3DRS_CCW_STENCILZFAIL:
        updateMemory(pso.DepthStencilState.BackFace.StencilDepthFailOp, (D3D12_STENCIL_OP)Value);
        break;

    case D3DRS_CCW_STENCILPASS:
        updateMemory(pso.DepthStencilState.BackFace.StencilPassOp, (D3D12_STENCIL_OP)Value);
        break;

    case D3DRS_CCW_STENCILFUNC:
        updateMemory(pso.DepthStencilState.BackFace.StencilFunc, (D3D12_COMPARISON_FUNC)Value);
        break;
        */
    case D3DRS_COLORWRITEENABLE1:
        updateMemoryAndNotify(pso.BlendState.RenderTarget[1].RenderTargetWriteMask, (UINT8)Value);
        break;

    case D3DRS_COLORWRITEENABLE2:
        updateMemoryAndNotify(pso.BlendState.RenderTarget[2].RenderTargetWriteMask, (UINT8)Value);
        break;

    case D3DRS_COLORWRITEENABLE3: 
        updateMemoryAndNotify(pso.BlendState.RenderTarget[3].RenderTargetWriteMask, (UINT8)Value);
        break;

    case D3DRS_DEPTHBIAS:
        break;

    case D3DRS_SRCBLENDALPHA:
        updateMemoryAndNotify(pso.BlendState.RenderTarget[0].SrcBlendAlpha, (D3D12_BLEND)Value);
        break;

    case D3DRS_DESTBLENDALPHA:
        updateMemoryAndNotify(pso.BlendState.RenderTarget[0].DestBlendAlpha, (D3D12_BLEND)Value);
        break;

    case D3DRS_BLENDOPALPHA:
        updateMemoryAndNotify(pso.BlendState.RenderTarget[0].BlendOpAlpha, (D3D12_BLEND_OP)Value);
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
        submitDraw();

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
        updateMemoryAndNotify(samplers[Sampler].AddressU, (D3D12_TEXTURE_ADDRESS_MODE)Value);
        break;

    case D3DSAMP_ADDRESSV:
        updateMemoryAndNotify(samplers[Sampler].AddressV, (D3D12_TEXTURE_ADDRESS_MODE)Value);
        break;

    case D3DSAMP_ADDRESSW:
        updateMemoryAndNotify(samplers[Sampler].AddressW, (D3D12_TEXTURE_ADDRESS_MODE)Value);
        break;

    case D3DSAMP_MAGFILTER:
        if (Value == D3DTEXF_LINEAR)
            updateMemoryAndNotify(samplers[Sampler].Filter, (D3D12_FILTER)(samplers[Sampler].Filter | 0x4));
        else
            updateMemoryAndNotify(samplers[Sampler].Filter, (D3D12_FILTER)(samplers[Sampler].Filter & ~0x4));

        break;

    case D3DSAMP_MINFILTER:
        if (Value == D3DTEXF_LINEAR)
            updateMemoryAndNotify(samplers[Sampler].Filter, (D3D12_FILTER)(samplers[Sampler].Filter | 0x10));
        else
            updateMemoryAndNotify(samplers[Sampler].Filter, (D3D12_FILTER)(samplers[Sampler].Filter & ~0x10));

        break;

    case D3DSAMP_MIPFILTER:
        if (Value == D3DTEXF_LINEAR)
            updateMemoryAndNotify(samplers[Sampler].Filter, (D3D12_FILTER)(samplers[Sampler].Filter | 0x1));
        else
            updateMemoryAndNotify(samplers[Sampler].Filter, (D3D12_FILTER)(samplers[Sampler].Filter & ~0x1));

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

    updateMemoryAndNotify(&scissorRect, &newScissorRect, sizeof(D3D12_RECT));
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

    requestDraw();
    renderQueue.getCommandList()->IASetPrimitiveTopology((D3D12_PRIMITIVE_TOPOLOGY)PrimitiveType);
    renderQueue.getCommandList()->DrawInstanced(PrimitiveCount, 1, StartVertex, 0);

    return S_OK;
}       
        
HRESULT Device::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
    if (PrimitiveType > D3DPT_TRIANGLESTRIP)
        return S_OK;

    requestDraw();
    renderQueue.getCommandList()->IASetPrimitiveTopology((D3D12_PRIMITIVE_TOPOLOGY)PrimitiveType);
    renderQueue.getCommandList()->DrawIndexedInstanced(primCount, 1, startIndex, BaseVertexIndex, 0);
    pendingDraw = true;

    return S_OK;
}       
        
HRESULT Device::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    if (PrimitiveType > D3DPT_TRIANGLESTRIP)
        return S_OK;

    // Calculate vertex count from primitive type
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

    // Calculate total byte size
    UINT totalSize = vertexCount * VertexStreamZeroStride;
    
    // Reserve space in the vertex buffer
    reserveVertexBuffer(totalSize);

    // Copy data to the vertex buffer
    updateMemoryAndNotify(vertexUploadBufferData, pVertexStreamZeroData, totalSize);

    // Initialize first vertex buffer view
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    vertexBufferView.BufferLocation = vertexUploadBuffer->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = totalSize;
    vertexBufferView.StrideInBytes = VertexStreamZeroStride;

    updateMemoryAndNotify(&vertexBufferViews[0], &vertexBufferView, sizeof(D3D12_VERTEX_BUFFER_VIEW));

    // Draw
    requestDraw();
    renderQueue.getCommandList()->IASetPrimitiveTopology((D3D12_PRIMITIVE_TOPOLOGY)PrimitiveType);
    renderQueue.getCommandList()->DrawInstanced(vertexCount, 1, 0, 0);
    pendingDraw = true;

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
        submitDraw();

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
        submitDraw();

    vertexShader = pShader;
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShader, VertexShader** ppShader)

HRESULT Device::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    updateMemoryAndNotify(&vertexConstants.getData()->c[StartRegister], pConstantData, Vector4fCount * sizeof(FLOAT[4]));
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantF, UINT StartRegister, float* pConstantData, UINT Vector4fCount)

HRESULT Device::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    updateMemoryAndNotify(&vertexConstants.getData()->i[StartRegister], pConstantData, Vector4iCount * sizeof(INT[4]));
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantI, UINT StartRegister, int* pConstantData, UINT Vector4iCount)

HRESULT Device::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
    updateMemoryAndNotify(&vertexConstants.getData()->b[StartRegister], pConstantData, BoolCount * sizeof(BOOL));
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantB, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)

HRESULT Device::SetStreamSource(UINT StreamNumber, VertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride)
{
    if (vertexBuffers[StreamNumber].Get() != pStreamData)
        submitDraw();

    vertexBuffers[StreamNumber] = pStreamData;

    if (pStreamData)
        vertexBufferViews[StreamNumber] = pStreamData->getVertexBufferView(OffsetInBytes, Stride);
    else
        memset(&vertexBufferViews[StreamNumber], 0, sizeof(D3D12_VERTEX_BUFFER_VIEW));

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
        submitDraw();

    indexBuffer = pIndexData;

    if (pIndexData)
        indexBufferView = pIndexData->getIndexBufferView();
    else
        memset(&indexBufferView, 0, sizeof(D3D12_VERTEX_BUFFER_VIEW));

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
        submitDraw();

    pixelShader = pShader;
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShader, PixelShader** ppShader)

HRESULT Device::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    updateMemoryAndNotify(&pixelConstants.getData()->c[StartRegister], pConstantData, Vector4fCount * sizeof(FLOAT[4]));
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantF, UINT StartRegister, float* pConstantData, UINT Vector4fCount)

HRESULT Device::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    updateMemoryAndNotify(&pixelConstants.getData()->i[StartRegister], pConstantData, Vector4iCount * sizeof(INT[4]));
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantI, UINT StartRegister, int* pConstantData, UINT Vector4iCount)

HRESULT Device::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
    updateMemoryAndNotify(&pixelConstants.getData()->b[StartRegister], pConstantData, BoolCount * sizeof(BOOL));
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantB, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)

FUNCTION_STUB(HRESULT, Device::DrawRectPatch, UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)

FUNCTION_STUB(HRESULT, Device::DrawTriPatch, UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)

FUNCTION_STUB(HRESULT, Device::DeletePatch, UINT Handle)

FUNCTION_STUB(HRESULT, Device::CreateQuery, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)