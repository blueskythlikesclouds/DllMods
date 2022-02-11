#include "Device.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Surface.h"
#include "TypeConverter.h"
#include "DepthStencilTexture.h"
#include "RenderTargetTexture.h"
#include "VertexDeclaration.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "RenderTargetSurface.h"
#include "DepthStencilSurface.h"

#define g_main g_pixel_main
#include "PixelShader.h"
#undef g_main
#define g_main g_vertex_main
#include "VertexShader.h"
#undef g_main

struct VertexShaderConstants
{
    FLOAT c[256][4];
    INT i[16][4];
    BOOL b[16];
};

struct PixelShaderConstants
{
    FLOAT c[224][4];
    INT i[16][4];
    BOOL b[16];
};

void Device::validateState()
{
    size_t renderTargetCount;
    for (renderTargetCount = 0; renderTargetCount < _countof(renderTargets) && renderTargets[renderTargetCount].ptr; renderTargetCount++)
        ;

    size_t vertexBufferCount;
    for (vertexBufferCount = 0; vertexBufferCount < _countof(vertexBufferViews) && vertexBufferViews[vertexBufferCount].BufferLocation; vertexBufferCount++)
        ;

    pso.NumRenderTargets = renderTargetCount;

    ID3D12GraphicsCommandList* commandList = renderQueue.getCommandList();

    commandList->SetGraphicsRootSignature(rootSignature.Get());
    commandList->SetGraphicsRootConstantBufferView(0, vertexShaderConstants.getResource()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(1, pixelShaderConstants.getResource()->GetGPUVirtualAddress());
    commandList->OMSetRenderTargets(renderTargetCount, renderTargets, FALSE, &depthStencil);
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);
    commandList->IASetIndexBuffer(&indexBufferView);
    commandList->IASetVertexBuffers(0, vertexBufferCount, vertexBufferViews);

    const size_t hash = generateCrc32Hash(0, &pso, sizeof(pso));

    ComPtr<ID3D12PipelineState> pipelineState;

    const auto pair = psoMap.find(hash);

    if (pair != psoMap.end())
        pipelineState = pair->second;

    else
    {
        device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineState));
        psoMap.insert(std::make_pair(hash, pipelineState));
    }

    commandList->SetPipelineState(pipelineState.Get());
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

    // Create root parameters
    D3D12_ROOT_PARAMETER rootParameters[2];

    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].Descriptor.ShaderRegister = 0;
    rootParameters[0].Descriptor.RegisterSpace = 0;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].Descriptor.ShaderRegister = 1;
    rootParameters[1].Descriptor.RegisterSpace = 0;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // Create root signature
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    
    ComPtr<ID3DBlob> rootSignatureBlob, errorBlob;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSignatureBlob, &errorBlob);
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
    vertexShaderConstants.initialize(device);
    pixelShaderConstants.initialize(device);

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

    // Create fullscreen depth stencil texture
    ComPtr<ID3D12Resource> depthStencilTex;
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = swapChainDesc.Width;
    depthStencilDesc.Height = swapChainDesc.Height;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
                                       &depthStencilDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
                                       &CD3DX12_CLEAR_VALUE(depthStencilDesc.Format, 1.0f, 0),
                                       IID_PPV_ARGS(&depthStencilTex));

    backBufferDepthStencil = new DepthStencilTexture(this, depthStencilTex);
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

    // Set render target and depth stencil
    renderQueue.getCommandList()->OMSetRenderTargets(1,
        &backBufferRenderTargets[backBufferIndex]->getDescriptorHandle(), FALSE, &backBufferDepthStencil->getDescriptorHandle());

    // Clear render target to black.
    FLOAT color[4] = { 0, 0, 0, 1 };
    renderQueue.getCommandList()->ClearRenderTargetView(backBufferRenderTargets[backBufferIndex]->getDescriptorHandle(), color, 0, nullptr);

    // Clear depth stencil
    renderQueue.getCommandList()->ClearDepthStencilView(backBufferDepthStencil->getDescriptorHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

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
    DXGI_FORMAT format = TypeConverter::makeUntypeless(TypeConverter::convert(Format), false);

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

FUNCTION_STUB(HRESULT, Device::CreateRenderTarget, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, Surface** ppSurface, HANDLE* pSharedHandle)

FUNCTION_STUB(HRESULT, Device::CreateDepthStencilSurface, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, Surface** ppSurface, HANDLE* pSharedHandle)

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
        renderTargets[RenderTargetIndex] = renderTargetTexture->getDescriptorHandle();
        pso.RTVFormats[RenderTargetIndex] = renderTargetTexture->getFormat();
    }

    else
    {
        ZeroMemory(&renderTargets[RenderTargetIndex], sizeof(renderTargets[RenderTargetIndex]));
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
        depthStencil = depthStencilTexture->getDescriptorHandle();
        pso.DSVFormat = depthStencilTexture->getFormat();
    }

    else
    {
        ZeroMemory(&depthStencil, sizeof(depthStencil));
        pso.DSVFormat = DXGI_FORMAT_UNKNOWN;
    }

    return S_OK;
}

HRESULT Device::GetDepthStencilSurface(Surface** ppZStencilSurface)
{

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

        for (size_t i = 0; i < Count; i++)
            commandList->ClearRenderTargetView(renderTargets[i], color, 0, nullptr);
    }

    if (Flags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL))
    {
        D3D12_CLEAR_FLAGS clearFlags = (D3D12_CLEAR_FLAGS)0;

        if (Flags & D3DCLEAR_ZBUFFER)
            clearFlags |= D3D12_CLEAR_FLAG_DEPTH;

        if (Flags & D3DCLEAR_STENCIL)
            clearFlags |= D3D12_CLEAR_FLAG_STENCIL;

        commandList->ClearDepthStencilView(depthStencil, clearFlags, Z, (UINT8)Stencil, 0, nullptr);
    }

    renderQueue.submitAll();

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::SetTransform, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)

FUNCTION_STUB(HRESULT, Device::GetTransform, D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)

FUNCTION_STUB(HRESULT, Device::MultiplyTransform, D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*)

HRESULT Device::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
    // Create D3D12_VIEWPORT from D3DVIEWPORT9
    viewport.TopLeftX = (float)pViewport->X;
    viewport.TopLeftY = (float)pViewport->Y;
    viewport.Width = (float)pViewport->Width;
    viewport.Height = (float)pViewport->Height;
    viewport.MinDepth = pViewport->MinZ;
    viewport.MaxDepth = pViewport->MaxZ;
    return S_OK;
}

HRESULT Device::GetViewport(D3DVIEWPORT9* pViewport)
{
    // Create D3DVIEWPORT9 from D3D12_VIEWPORT
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
        pso.DepthStencilState.DepthEnable = (BOOL)Value;
        break;

    case D3DRS_FILLMODE:
        pso.RasterizerState.FillMode = (D3D12_FILL_MODE)Value;
        break;

    case D3DRS_ZWRITEENABLE:
        pso.DepthStencilState.DepthWriteMask = (D3D12_DEPTH_WRITE_MASK)Value;
        break;

    case D3DRS_SRCBLEND:
        pso.BlendState.RenderTarget[0].SrcBlend = (D3D12_BLEND)Value;
        break;

    case D3DRS_DESTBLEND:
        pso.BlendState.RenderTarget[0].DestBlend = (D3D12_BLEND)Value;
        break;

    case D3DRS_CULLMODE:
        pso.RasterizerState.CullMode = (D3D12_CULL_MODE)Value;
        break;

    case D3DRS_ZFUNC:
        pso.DepthStencilState.DepthFunc = (D3D12_COMPARISON_FUNC)Value;
        break;

    case D3DRS_ALPHABLENDENABLE:
        pso.BlendState.RenderTarget[0].BlendEnable = (BOOL)Value;
        break;

    case D3DRS_STENCILENABLE:
        pso.DepthStencilState.StencilEnable = (BOOL)Value;
        break;

    case D3DRS_STENCILFAIL:
        pso.DepthStencilState.FrontFace.StencilFailOp = (D3D12_STENCIL_OP)Value;
        break;

    case D3DRS_STENCILZFAIL:
        pso.DepthStencilState.FrontFace.StencilDepthFailOp = (D3D12_STENCIL_OP)Value;
        break;

    case D3DRS_STENCILPASS:
        pso.DepthStencilState.FrontFace.StencilPassOp = (D3D12_STENCIL_OP)Value;
        break;

    case D3DRS_STENCILFUNC:
        pso.DepthStencilState.FrontFace.StencilFunc = (D3D12_COMPARISON_FUNC)Value;
        break;

    case D3DRS_STENCILREF:
        stencilRef = (UINT)Value;
        break;

    case D3DRS_STENCILMASK:
        pso.DepthStencilState.StencilReadMask = (UINT8)Value;
        break;

    case D3DRS_STENCILWRITEMASK:
        pso.DepthStencilState.StencilWriteMask = (UINT8)Value;
        break;

    case D3DRS_COLORWRITEENABLE:
        pso.BlendState.RenderTarget[0].RenderTargetWriteMask = (UINT8)Value;
        break;

    case D3DRS_BLENDOP:
        pso.BlendState.RenderTarget[0].BlendOp = (D3D12_BLEND_OP)Value;
        break;

    case D3DRS_SLOPESCALEDEPTHBIAS:
        pso.RasterizerState.SlopeScaledDepthBias = *(float*)&Value;
        break;

    case D3DRS_CCW_STENCILFAIL:
        pso.DepthStencilState.BackFace.StencilFailOp = (D3D12_STENCIL_OP)Value;
        break;

    case D3DRS_CCW_STENCILZFAIL:
        pso.DepthStencilState.BackFace.StencilDepthFailOp = (D3D12_STENCIL_OP)Value;
        break;

    case D3DRS_CCW_STENCILPASS:
        pso.DepthStencilState.BackFace.StencilPassOp = (D3D12_STENCIL_OP)Value;
        break;

    case D3DRS_CCW_STENCILFUNC:
        pso.DepthStencilState.BackFace.StencilFunc = (D3D12_COMPARISON_FUNC)Value;
        break;

    case D3DRS_COLORWRITEENABLE1:
        pso.BlendState.RenderTarget[1].RenderTargetWriteMask = (UINT8)Value;
        break;

    case D3DRS_COLORWRITEENABLE2:
        pso.BlendState.RenderTarget[2].RenderTargetWriteMask = (UINT8)Value;
        break;

    case D3DRS_COLORWRITEENABLE3: 
        pso.BlendState.RenderTarget[3].RenderTargetWriteMask = (UINT8)Value;
        break;

    case D3DRS_DEPTHBIAS:
        break;

    case D3DRS_SRCBLENDALPHA:
        pso.BlendState.RenderTarget[0].SrcBlendAlpha = (D3D12_BLEND)Value;
        break;

    case D3DRS_DESTBLENDALPHA:
        pso.BlendState.RenderTarget[0].DestBlendAlpha = (D3D12_BLEND)Value;
        break;

    case D3DRS_BLENDOPALPHA:
        pso.BlendState.RenderTarget[0].BlendOpAlpha = (D3D12_BLEND_OP)Value;
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
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::ValidateDevice, DWORD* pNumPasses)

FUNCTION_STUB(HRESULT, Device::SetPaletteEntries, UINT PaletteNumber, CONST PALETTEENTRY* pEntries)

FUNCTION_STUB(HRESULT, Device::GetPaletteEntries, UINT PaletteNumber, PALETTEENTRY* pEntries)

FUNCTION_STUB(HRESULT, Device::SetCurrentTexturePalette, UINT PaletteNumber)

FUNCTION_STUB(HRESULT, Device::GetCurrentTexturePalette, UINT *PaletteNumber)

HRESULT Device::SetScissorRect(CONST RECT* pRect)
{
    // Create D3D12_RECT from RECT
    scissorRect.left = pRect->left;
    scissorRect.top = pRect->top;
    scissorRect.right = pRect->right;
    scissorRect.bottom = pRect->bottom;
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

    validateState();
    renderQueue.getCommandList()->IASetPrimitiveTopology((D3D12_PRIMITIVE_TOPOLOGY)PrimitiveType);
    renderQueue.getCommandList()->DrawInstanced(PrimitiveCount, 1, StartVertex, 0);
    renderQueue.submitAll();

    return S_OK;
}       
        
HRESULT Device::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
    if (PrimitiveType > D3DPT_TRIANGLESTRIP)
        return S_OK;

    validateState();

    renderQueue.getCommandList()->IASetPrimitiveTopology((D3D12_PRIMITIVE_TOPOLOGY)PrimitiveType);
    renderQueue.getCommandList()->DrawIndexedInstanced(primCount, 1, startIndex, BaseVertexIndex, 0);
    renderQueue.submitAll();

    return S_OK;
}       
        
HRESULT Device::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{       
    return S_OK;
}       
        
HRESULT Device::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::ProcessVertices, UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, VertexBuffer* pDestBuffer, VertexDeclaration* pVertexDecl, DWORD Flags)

HRESULT Device::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, VertexDeclaration** ppDecl)
{
    *ppDecl = new VertexDeclaration(pVertexElements);
    return S_OK;
}

HRESULT Device::SetVertexDeclaration(VertexDeclaration* pDecl)
{
    pso.InputLayout = pDecl ? pDecl->getInputLayoutDesc() : D3D12_INPUT_LAYOUT_DESC();
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
    //*ppShader = new VertexShader(pFunction, FunctionSize);
    *ppShader = new VertexShader(g_vertex_main, sizeof(g_vertex_main));
    return S_OK;
}

HRESULT Device::SetVertexShader(VertexShader* pShader)
{
    pso.VS = pShader ? pShader->getShaderByteCode() : D3D12_SHADER_BYTECODE();
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShader, VertexShader** ppShader)

HRESULT Device::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    memcpy(&vertexShaderConstants.getData()->c[StartRegister], pConstantData, Vector4fCount * sizeof(FLOAT[4]));
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantF, UINT StartRegister, float* pConstantData, UINT Vector4fCount)

HRESULT Device::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    memcpy(&vertexShaderConstants.getData()->i[StartRegister], pConstantData, Vector4iCount * sizeof(INT[4]));
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantI, UINT StartRegister, int* pConstantData, UINT Vector4iCount)

HRESULT Device::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
    memcpy(&vertexShaderConstants.getData()->b[StartRegister], pConstantData, BoolCount * sizeof(BOOL));
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantB, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)

HRESULT Device::SetStreamSource(UINT StreamNumber, VertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride)
{
    vertexBufferViews[StreamNumber] = pStreamData ? pStreamData->getVertexBufferView(OffsetInBytes, Stride) : D3D12_VERTEX_BUFFER_VIEW();
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
    indexBufferView = pIndexData ? pIndexData->getIndexBufferView() : D3D12_INDEX_BUFFER_VIEW();
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetIndices, IndexBuffer** ppIndexData)

HRESULT Device::CreatePixelShader(CONST DWORD* pFunction, PixelShader** ppShader, DWORD FunctionSize)
{
    //*ppShader = new PixelShader(pFunction, FunctionSize);
    *ppShader = new PixelShader(g_pixel_main, sizeof(g_pixel_main));
    return S_OK;
}

HRESULT Device::SetPixelShader(PixelShader* pShader)
{
    pso.PS = pShader ? pShader->getShaderByteCode() : D3D12_SHADER_BYTECODE();
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShader, PixelShader** ppShader)

HRESULT Device::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    memcpy(&pixelShaderConstants.getData()->c[StartRegister], pConstantData, Vector4fCount * sizeof(FLOAT[4]));
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantF, UINT StartRegister, float* pConstantData, UINT Vector4fCount)

HRESULT Device::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    memcpy(&pixelShaderConstants.getData()->i[StartRegister], pConstantData, Vector4iCount * sizeof(INT[4]));
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantI, UINT StartRegister, int* pConstantData, UINT Vector4iCount)

HRESULT Device::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
    memcpy(&pixelShaderConstants.getData()->b[StartRegister], pConstantData, BoolCount * sizeof(BOOL));
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantB, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)

FUNCTION_STUB(HRESULT, Device::DrawRectPatch, UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)

FUNCTION_STUB(HRESULT, Device::DrawTriPatch, UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)

FUNCTION_STUB(HRESULT, Device::DeletePatch, UINT Handle)

FUNCTION_STUB(HRESULT, Device::CreateQuery, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)