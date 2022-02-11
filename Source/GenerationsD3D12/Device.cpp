#include "Device.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Surface.h"
#include "TypeConverter.h"
#include "DepthStencilTexture.h"
#include "Hash.h"
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
    for (renderTargetCount = 0; renderTargetCount < _countof(d3dRenderTargets) && d3dRenderTargets[renderTargetCount].ptr; renderTargetCount++)
        ;

    size_t vertexBufferCount;
    for (vertexBufferCount = 0; vertexBufferCount < _countof(d3dVertexBufferViews) && d3dVertexBufferViews[vertexBufferCount].BufferLocation; vertexBufferCount++)
        ;

    d3dPipelineStateDesc.NumRenderTargets = renderTargetCount;

    ID3D12GraphicsCommandList* d3dCommandList = renderQueue.getD3DCommandList();

    d3dCommandList->SetGraphicsRootSignature(d3dRootSignature.Get());
    d3dCommandList->SetGraphicsRootConstantBufferView(0, vertexShaderConstants.getD3DResource()->GetGPUVirtualAddress());
    d3dCommandList->SetGraphicsRootConstantBufferView(1, pixelShaderConstants.getD3DResource()->GetGPUVirtualAddress());
    d3dCommandList->OMSetRenderTargets(renderTargetCount, d3dRenderTargets, FALSE, &d3dDepthStencil);
    d3dCommandList->RSSetViewports(1, &d3dViewport);
    d3dCommandList->RSSetScissorRects(1, &d3dScissorRect);
    d3dCommandList->IASetIndexBuffer(&d3dIndexBufferView);
    d3dCommandList->IASetVertexBuffers(0, vertexBufferCount, d3dVertexBufferViews);

    const size_t hash = Hash::compute(&d3dPipelineStateDesc, sizeof(d3dPipelineStateDesc));

    ComPtr<ID3D12PipelineState> d3dPipelineState;

    const auto pair = d3dPipelineStates.find(hash);

    if (pair != d3dPipelineStates.end())
        d3dPipelineState = pair->second;

    else
    {
        d3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, IID_PPV_ARGS(&d3dPipelineState));
        d3dPipelineStates.insert(std::make_pair(hash, d3dPipelineState));
    }

    d3dCommandList->SetPipelineState(d3dPipelineState.Get());
}

Device::Device(D3DPRESENT_PARAMETERS* presentationParameters)
{
#if _DEBUG
    ComPtr<ID3D12Debug> d3dDebugInterface;
    D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugInterface));
    d3dDebugInterface->EnableDebugLayer();

    SetWindowLongPtr(presentationParameters->hDeviceWindow, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
    SetWindowPos(presentationParameters->hDeviceWindow, HWND_TOP, (1920 - 1600) / 2, (1080 - 900) / 2, 1600, 900, SWP_FRAMECHANGED);
#endif

    // Create device
    D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice));

    // Create DXGI factory
    ComPtr<IDXGIFactory4> dxgiFactory;
    CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

    // Create command queues
    renderQueue.initialize(d3dDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
    loadQueue.initialize(d3dDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);

    // Create swap chain
    ComPtr<IDXGISwapChain1> dxgiSwapChain1;

    DXGI_SWAP_CHAIN_DESC1 d3d12SwapChainDesc;
    d3d12SwapChainDesc.Width = presentationParameters->BackBufferWidth;
    d3d12SwapChainDesc.Height = presentationParameters->BackBufferHeight;
    d3d12SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    d3d12SwapChainDesc.Stereo = FALSE;
    d3d12SwapChainDesc.SampleDesc.Count = 1;
    d3d12SwapChainDesc.SampleDesc.Quality = 0;
    d3d12SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    d3d12SwapChainDesc.BufferCount = 2;
    d3d12SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    d3d12SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    d3d12SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    d3d12SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    dxgiFactory->CreateSwapChainForHwnd(renderQueue.getD3DCommandQueue(), presentationParameters->hDeviceWindow, &d3d12SwapChainDesc, nullptr, nullptr, &dxgiSwapChain1);
    dxgiSwapChain1.As(&dxgiSwapChain);

    // Create root parameters
    D3D12_ROOT_PARAMETER d3dRootParameters[2];

    d3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    d3dRootParameters[0].Descriptor.ShaderRegister = 0;
    d3dRootParameters[0].Descriptor.RegisterSpace = 0;
    d3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    d3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    d3dRootParameters[1].Descriptor.ShaderRegister = 1;
    d3dRootParameters[1].Descriptor.RegisterSpace = 0;
    d3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // Create root signature
    D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
    d3dRootSignatureDesc.NumParameters = _countof(d3dRootParameters);
    d3dRootSignatureDesc.pParameters = d3dRootParameters;
    d3dRootSignatureDesc.NumStaticSamplers = 0;
    d3dRootSignatureDesc.pStaticSamplers = nullptr;
    d3dRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    
    ComPtr<ID3DBlob> d3dRootSignatureBlob, d3dErrorBlob;
    D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &d3dRootSignatureBlob, &d3dErrorBlob);
    d3dDevice->CreateRootSignature(0, d3dRootSignatureBlob->GetBufferPointer(), d3dRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&d3dRootSignature));

    // Initialize d3dPipelineStateDesc
    d3dPipelineStateDesc.pRootSignature = d3dRootSignature.Get();
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dPipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    d3dPipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    d3dPipelineStateDesc.SampleDesc = d3d12SwapChainDesc.SampleDesc;
    d3dPipelineStateDesc.SampleMask = ~0;
    d3dPipelineStateDesc.NumRenderTargets = 1;
    d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    d3dPipelineStateDesc.DepthStencilState.DepthEnable = true;
    d3dPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    d3dPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

    // Initialize constant buffers
    vertexShaderConstants.initialize(d3dDevice);
    pixelShaderConstants.initialize(d3dDevice);

    // Create swap chain render targets
    for (int i = 0; i < _countof(d3dSwapChainRenderTargets); i++)
    {
        ComPtr<ID3D12Resource> d3dBackBuffer;
        dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&d3dBackBuffer));
        d3dSwapChainRenderTargets[i] = new RenderTargetTexture(this, d3dBackBuffer, d3dBackBuffer->GetDesc().Format);
    }

    d3dSwapChainRenderTargetIndex = dxgiSwapChain->GetCurrentBackBufferIndex();

    // Transition from present to render target
    renderQueue.getD3DCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
                                                       d3dSwapChainRenderTargets[d3dSwapChainRenderTargetIndex]->getD3DResource(),
                                                       D3D12_RESOURCE_STATE_PRESENT,
                                                       D3D12_RESOURCE_STATE_RENDER_TARGET));

    // Create fullscreen depth stencil texture
    ComPtr<ID3D12Resource> d3dDepthStencilTexture;
    D3D12_RESOURCE_DESC d3dDepthStencilTextureDesc;
    d3dDepthStencilTextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    d3dDepthStencilTextureDesc.Alignment = 0;
    d3dDepthStencilTextureDesc.Width = d3d12SwapChainDesc.Width;
    d3dDepthStencilTextureDesc.Height = d3d12SwapChainDesc.Height;
    d3dDepthStencilTextureDesc.DepthOrArraySize = 1;
    d3dDepthStencilTextureDesc.MipLevels = 1;
    d3dDepthStencilTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
    d3dDepthStencilTextureDesc.SampleDesc.Count = 1;
    d3dDepthStencilTextureDesc.SampleDesc.Quality = 0;
    d3dDepthStencilTextureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    d3dDepthStencilTextureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    d3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
                                       &d3dDepthStencilTextureDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
                                       &CD3DX12_CLEAR_VALUE(d3dDepthStencilTextureDesc.Format, 1.0f, 0),
                                       IID_PPV_ARGS(&d3dDepthStencilTexture));

    d3dSwapChainDepthStencil = new DepthStencilTexture(this, d3dDepthStencilTexture, d3dDepthStencilTextureDesc.Format);
}

ID3D12Device* Device::getD3DDevice() const
{
    return d3dDevice.Get();
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
    renderQueue.getD3DCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
                                                       d3dSwapChainRenderTargets[d3dSwapChainRenderTargetIndex]->getD3DResource(),
                                                       D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                       D3D12_RESOURCE_STATE_PRESENT));

    // Submit all
    renderQueue.submitAll();

    // Present
    dxgiSwapChain->Present(1, 0);

    // Set render target index
    d3dSwapChainRenderTargetIndex = dxgiSwapChain->GetCurrentBackBufferIndex();

    // Transition from present to render target
    renderQueue.getD3DCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
                                                       d3dSwapChainRenderTargets[d3dSwapChainRenderTargetIndex]->getD3DResource(),
                                                       D3D12_RESOURCE_STATE_PRESENT,
                                                       D3D12_RESOURCE_STATE_RENDER_TARGET));

    // Set render target and depth stencil
    renderQueue.getD3DCommandList()->OMSetRenderTargets(1,
        &d3dSwapChainRenderTargets[d3dSwapChainRenderTargetIndex]->getD3DCpuDescriptorHandle(), FALSE, &d3dSwapChainDepthStencil->getD3DCpuDescriptorHandle());

    // Clear render target to black.
    FLOAT color[4] = { 0, 0, 0, 1 };
    renderQueue.getD3DCommandList()->ClearRenderTargetView(d3dSwapChainRenderTargets[d3dSwapChainRenderTargetIndex]->getD3DCpuDescriptorHandle(), color, 0, nullptr);

    // Clear depth stencil
    renderQueue.getD3DCommandList()->ClearDepthStencilView(d3dSwapChainDepthStencil->getD3DCpuDescriptorHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    return S_OK;
}

HRESULT Device::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, Surface** ppBackBuffer)
{
    d3dSwapChainRenderTargets[d3dSwapChainRenderTargetIndex]->GetSurfaceLevel(0, ppBackBuffer);
    return S_OK;
}
    
FUNCTION_STUB(HRESULT, Device::GetRasterStatus, UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)

FUNCTION_STUB(HRESULT, Device::SetDialogBoxMode, BOOL bEnableDialogs)

FUNCTION_STUB(void, Device::SetGammaRamp, UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)

FUNCTION_STUB(void, Device::GetGammaRamp, UINT iSwapChain, D3DGAMMARAMP* pRamp)

HRESULT Device::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, Texture** ppTexture, HANDLE* pSharedHandle)
{
    ComPtr<ID3D12Resource> d3dTexture;
    DXGI_FORMAT format = TypeConverter::makeUntypeless(TypeConverter::convert(Format), false);

    FLOAT transparent[4] = { 0, 0, 0, 1 };

    const HRESULT hr = d3dDevice->CreateCommittedResource
    (
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),

        D3D12_HEAP_FLAG_NONE,

        &CD3DX12_RESOURCE_DESC::Tex2D(format, Width, Height, 1, Levels, 1, 0,
            Usage & D3DUSAGE_RENDERTARGET ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : 
            Usage & D3DUSAGE_DEPTHSTENCIL ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE),

        Usage & D3DUSAGE_RENDERTARGET ? D3D12_RESOURCE_STATE_RENDER_TARGET :
        Usage & D3DUSAGE_DEPTHSTENCIL ? D3D12_RESOURCE_STATE_DEPTH_WRITE : D3D12_RESOURCE_STATE_COMMON,

        Usage & D3DUSAGE_RENDERTARGET ? &CD3DX12_CLEAR_VALUE(format, transparent) :
        Usage & D3DUSAGE_DEPTHSTENCIL ? &CD3DX12_CLEAR_VALUE(format, 1.0f, 0) : nullptr,

        IID_PPV_ARGS(&d3dTexture)
    );
        
    if (FAILED(hr))
        return hr;

    if (Usage & D3DUSAGE_RENDERTARGET)
        *ppTexture = new RenderTargetTexture(this, d3dTexture, d3dTexture->GetDesc().Format);

    else if (Usage & D3DUSAGE_DEPTHSTENCIL)
        *ppTexture = new DepthStencilTexture(this, d3dTexture, d3dTexture->GetDesc().Format);

    else
        *ppTexture = new Texture(this, d3dTexture);

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
        const auto renderTargetTexture = ((RenderTargetSurface*)pRenderTarget)->getRenderTargetTexture();
        d3dRenderTargets[RenderTargetIndex] = renderTargetTexture->getD3DCpuDescriptorHandle();
        d3dPipelineStateDesc.RTVFormats[RenderTargetIndex] = renderTargetTexture->getFormat();
    }

    else
    {
        ZeroMemory(&d3dRenderTargets[RenderTargetIndex], sizeof(d3dRenderTargets[RenderTargetIndex]));
        d3dPipelineStateDesc.RTVFormats[RenderTargetIndex] = DXGI_FORMAT_UNKNOWN;
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
        const auto depthStencilTexture = ((DepthStencilSurface*)pNewZStencil)->getDepthStencilTexture();
        d3dDepthStencil = depthStencilTexture->getD3DCpuDescriptorHandle();
        d3dPipelineStateDesc.DSVFormat = depthStencilTexture->getFormat();
    }

    else
    {
        ZeroMemory(&d3dDepthStencil, sizeof(d3dDepthStencil));
        d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
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
    ID3D12GraphicsCommandList* d3dCommandList = renderQueue.getD3DCommandList();

    if (Flags & D3DCLEAR_TARGET)
    {
        // Convert D3DCOLOR to FLOAT[4]
        FLOAT color[4];
        color[0] = ((Color >> 16) & 0xFF) / 255.0f;
        color[1] = ((Color >> 8) & 0xFF) / 255.0f;
        color[2] = (Color & 0xFF) / 255.0f;
        color[3] = ((Color >> 24) & 0xFF) / 255.0f;

        for (size_t i = 0; i < Count; i++)
            d3dCommandList->ClearRenderTargetView(d3dRenderTargets[i], color, 0, nullptr);
    }

    if (Flags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL))
    {
        D3D12_CLEAR_FLAGS clearFlags = (D3D12_CLEAR_FLAGS)0;

        if (Flags & D3DCLEAR_ZBUFFER)
            clearFlags |= D3D12_CLEAR_FLAG_DEPTH;

        if (Flags & D3DCLEAR_STENCIL)
            clearFlags |= D3D12_CLEAR_FLAG_STENCIL;

        d3dCommandList->ClearDepthStencilView(d3dDepthStencil, clearFlags, Z, (UINT8)Stencil, 0, nullptr);
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
    d3dViewport.TopLeftX = (float)pViewport->X;
    d3dViewport.TopLeftY = (float)pViewport->Y;
    d3dViewport.Width = (float)pViewport->Width;
    d3dViewport.Height = (float)pViewport->Height;
    d3dViewport.MinDepth = pViewport->MinZ;
    d3dViewport.MaxDepth = pViewport->MaxZ;
    return S_OK;
}

HRESULT Device::GetViewport(D3DVIEWPORT9* pViewport)
{
    // Create D3DVIEWPORT9 from D3D12_VIEWPORT
    pViewport->X = (DWORD)d3dViewport.TopLeftX;
    pViewport->Y = (DWORD)d3dViewport.TopLeftY;
    pViewport->Width = (DWORD)d3dViewport.Width;
    pViewport->Height = (DWORD)d3dViewport.Height;
    pViewport->MinZ = d3dViewport.MinDepth;
    pViewport->MaxZ = d3dViewport.MaxDepth;
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
    d3dScissorRect.left = pRect->left;
    d3dScissorRect.top = pRect->top;
    d3dScissorRect.right = pRect->right;
    d3dScissorRect.bottom = pRect->bottom;
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetScissorRect, RECT* pRect)

FUNCTION_STUB(HRESULT, Device::SetSoftwareVertexProcessing, BOOL bSoftware)

FUNCTION_STUB(BOOL, Device::GetSoftwareVertexProcessing)

FUNCTION_STUB(HRESULT, Device::SetNPatchMode, float nSegments)

FUNCTION_STUB(float, Device::GetNPatchMode)

HRESULT Device::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
    validateState();
    renderQueue.getD3DCommandList()->IASetPrimitiveTopology(TypeConverter::getPrimitiveTopology(PrimitiveType));
    renderQueue.getD3DCommandList()->DrawInstanced(PrimitiveCount, 1, StartVertex, 0);
    renderQueue.submitAll();

    return S_OK;
}       
        
HRESULT Device::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
    validateState();

    renderQueue.getD3DCommandList()->IASetPrimitiveTopology(TypeConverter::getPrimitiveTopology(PrimitiveType));
    renderQueue.getD3DCommandList()->DrawIndexedInstanced(primCount, 1, startIndex, BaseVertexIndex, 0);
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
    d3dPipelineStateDesc.InputLayout = pDecl ? pDecl->getD3DInputLayoutDesc() : D3D12_INPUT_LAYOUT_DESC();
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
    d3dPipelineStateDesc.VS = pShader ? pShader->getD3DShaderByteCode() : D3D12_SHADER_BYTECODE();
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
    d3dVertexBufferViews[StreamNumber] = pStreamData ? pStreamData->getD3DVertexBufferView(OffsetInBytes, Stride) : D3D12_VERTEX_BUFFER_VIEW();
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
    d3dIndexBufferView = pIndexData ? pIndexData->getD3DIndexBufferView() : D3D12_INDEX_BUFFER_VIEW();
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
    d3dPipelineStateDesc.PS = pShader ? pShader->getD3DShaderByteCode() : D3D12_SHADER_BYTECODE();
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