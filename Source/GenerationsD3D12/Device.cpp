#include "Device.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Surface.h"
#include "ConversionUtilities.h"
#include "RenderTargetTexture.h"
#include "VertexDeclaration.h"
#include "Texture.h"
#include "VertexBuffer.h"

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
    D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device));

    // Create command queue
    D3D12_COMMAND_QUEUE_DESC d3d12CommandQueueDesc;
    d3d12CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    d3d12CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    d3d12CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    d3d12CommandQueueDesc.NodeMask = 0;
    d3d12Device->CreateCommandQueue(&d3d12CommandQueueDesc, IID_PPV_ARGS(&d3d12CommandQueue));

    // Create command allocator
    d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3d12CommandAllocator));

    // Create graphics command list
    d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3d12CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&d3d12CommandList));

    // Create fence
    d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d12Fence));
    d3d12FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    // Create DXGI factory
    ComPtr<IDXGIFactory4> dxgiFactory;
    CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

    // Create swap chain
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
    dxgiFactory->CreateSwapChainForHwnd(d3d12CommandQueue.Get(), presentationParameters->hDeviceWindow, &d3d12SwapChainDesc, nullptr, nullptr, &dxgiSwapChain);

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
    return dxgiSwapChain->Present(1, 0);
}

HRESULT Device::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, Surface** ppBackBuffer)
{
    ComPtr<ID3D12Resource> d3d12BackBuffer;

    const HRESULT hr = dxgiSwapChain->GetBuffer(iBackBuffer, IID_PPV_ARGS(&d3d12BackBuffer));
    if (FAILED(hr))
        return hr;

    *ppBackBuffer = new Surface(this, d3d12BackBuffer);
    return S_OK;
}
    
FUNCTION_STUB(HRESULT, Device::GetRasterStatus, UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)

FUNCTION_STUB(HRESULT, Device::SetDialogBoxMode, BOOL bEnableDialogs)

FUNCTION_STUB(void, Device::SetGammaRamp, UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)

FUNCTION_STUB(void, Device::GetGammaRamp, UINT iSwapChain, D3DGAMMARAMP* pRamp)

HRESULT Device::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, Texture** ppTexture, HANDLE* pSharedHandle)
{
    ComPtr<ID3D12Resource> d3d12Texture;

    const CD3DX12_HEAP_PROPERTIES d3d12HeapProperties(D3D12_HEAP_TYPE_DEFAULT);
    const CD3DX12_RESOURCE_DESC d3d12ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(ConversionUtilities::convert(Format), Width, Height, 1, Levels);

    const HRESULT hr = d3d12Device->CreateCommittedResource(&d3d12HeapProperties, D3D12_HEAP_FLAG_NONE,
                                                            &d3d12ResourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr,
                                                            IID_PPV_ARGS(&d3d12Texture));
    if (FAILED(hr))
        return hr;

    if (Usage & D3DUSAGE_RENDERTARGET)
    {
    }
    else if (Usage & D3DUSAGE_DEPTHSTENCIL)
    {
    }
    else
    {
        *ppTexture = new Texture(this, d3d12Texture);
    }

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::CreateVolumeTexture, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)

FUNCTION_STUB(HRESULT, Device::CreateCubeTexture, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, CubeTexture** ppCubeTexture, HANDLE* pSharedHandle)

HRESULT Device::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, VertexBuffer** ppVertexBuffer, HANDLE* pSharedHandle)
{
    return S_OK;
}

HRESULT Device::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IndexBuffer** ppIndexBuffer, HANDLE* pSharedHandle)
{
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

    return S_OK;
}

HRESULT Device::GetRenderTarget(DWORD RenderTargetIndex, Surface** ppRenderTarget)
{

    return S_OK;
}

HRESULT Device::SetDepthStencilSurface(Surface* pNewZStencil)
{
    
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
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::SetTransform, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)

FUNCTION_STUB(HRESULT, Device::GetTransform, D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)

FUNCTION_STUB(HRESULT, Device::MultiplyTransform, D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*)

HRESULT Device::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
    d3d9Viewport = *pViewport;

    // Create D3D12_VIEWPORT from D3DVIEWPORT9
    D3D12_VIEWPORT d3d12Viewport;
    d3d12Viewport.TopLeftX = (float)pViewport->X;
    d3d12Viewport.TopLeftY = (float)pViewport->Y;
    d3d12Viewport.Width = (float)pViewport->Width;
    d3d12Viewport.Height = (float)pViewport->Height;
    d3d12Viewport.MinDepth = pViewport->MinZ;
    d3d12Viewport.MaxDepth = pViewport->MaxZ;

    // Set the viewport
    d3d12CommandList->RSSetViewports(1, &d3d12Viewport);
    return S_OK;
}

HRESULT Device::GetViewport(D3DVIEWPORT9* pViewport)
{
    *pViewport = d3d9Viewport;
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
    D3D12_RECT d3d12Rect;
    d3d12Rect.left = pRect->left;
    d3d12Rect.top = pRect->top;
    d3d12Rect.right = pRect->right;
    d3d12Rect.bottom = pRect->bottom;

    // Set the scissor rectangle
    d3d12CommandList->RSSetScissorRects(1, &d3d12Rect);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetScissorRect, RECT* pRect)

FUNCTION_STUB(HRESULT, Device::SetSoftwareVertexProcessing, BOOL bSoftware)

FUNCTION_STUB(BOOL, Device::GetSoftwareVertexProcessing)

FUNCTION_STUB(HRESULT, Device::SetNPatchMode, float nSegments)

FUNCTION_STUB(float, Device::GetNPatchMode)

HRESULT Device::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{       
    return S_OK;
}       
        
HRESULT Device::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{       
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
    return S_OK;
}

HRESULT Device::SetVertexShader(VertexShader* pShader)
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShader, VertexShader** ppShader)

HRESULT Device::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantF, UINT StartRegister, float* pConstantData, UINT Vector4fCount)

HRESULT Device::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantI, UINT StartRegister, int* pConstantData, UINT Vector4iCount)

HRESULT Device::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantB, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)

HRESULT Device::SetStreamSource(UINT StreamNumber, VertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride)
{
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
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetIndices, IndexBuffer** ppIndexData)

HRESULT Device::CreatePixelShader(CONST DWORD* pFunction, PixelShader** ppShader, DWORD FunctionSize)
{
    return S_OK;
}

HRESULT Device::SetPixelShader(PixelShader* pShader)
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShader, PixelShader** ppShader)

HRESULT Device::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantF, UINT StartRegister, float* pConstantData, UINT Vector4fCount)

HRESULT Device::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantI, UINT StartRegister, int* pConstantData, UINT Vector4iCount)

HRESULT Device::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantB, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)

FUNCTION_STUB(HRESULT, Device::DrawRectPatch, UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)

FUNCTION_STUB(HRESULT, Device::DrawTriPatch, UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)

FUNCTION_STUB(HRESULT, Device::DeletePatch, UINT Handle)

FUNCTION_STUB(HRESULT, Device::CreateQuery, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)