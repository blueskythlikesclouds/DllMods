#include "D3D9Ex.h"
#include "D3D9ExBaseTexture.h"
#include "D3D9ExCubeTexture.h"
#include "D3D9ExDevice.h"
#include "D3D9ExIndexBuffer.h"
#include "D3D9ExPixelShader.h"
#include "D3D9ExSurface.h"
#include "D3D9ExTexture.h"
#include "D3D9ExVertexBuffer.h"
#include "D3D9ExVertexDeclaration.h"
#include "D3D9ExVertexShader.h"

#include "Configuration.h"

D3D9ExDevice::D3D9ExDevice(D3D9Ex* dxp, IDirect3DDevice9Ex* d3dDevice)
    : d3dDevice(d3dDevice), dxp(dxp), dxpBackBuffer(nullptr), dxpRenderTargets {}, dxpDepthStencil(nullptr), dxpStageTextures{}, dxpVertexDeclaration(nullptr), dxpVertexShader(nullptr), dxpIndexBuffer(nullptr), dxpPixelShader(nullptr)
{
    d3dDevice->AddRef();
    dxp->AddRef();
}

D3D9ExDevice::~D3D9ExDevice()
{
    d3dDevice->Release();
    dxp->Release();

    if (dxpBackBuffer) 
        dxpBackBuffer->Release();

    for (auto& dxpRenderTarget : dxpRenderTargets)
        if (dxpRenderTarget) 
            dxpRenderTarget->Release();

    if (dxpDepthStencil) 
        dxpDepthStencil->Release();

    for (auto& dxpStageTexture : dxpStageTextures)
        if (dxpStageTexture) 
            dxpStageTexture->Release();

    if (dxpVertexDeclaration) 
        dxpVertexDeclaration->Release();

    for (auto& src : streamSources)
        if (src.dxpVertexBuffer) 
            src.dxpVertexBuffer->Release();

    if (dxpIndexBuffer) 
        dxpIndexBuffer->Release();

    if (dxpPixelShader)
        dxpPixelShader->Release();
}

HRESULT D3D9ExDevice::TestCooperativeLevel()
{
    return S_OK;
}

UINT D3D9ExDevice::GetAvailableTextureMem()
{
    return d3dDevice->GetAvailableTextureMem();
}

HRESULT D3D9ExDevice::EvictManagedResources()
{
    return d3dDevice->EvictManagedResources();
}

HRESULT D3D9ExDevice::GetDirect3D(D3D9Ex** ppD3D9)
{
    *ppD3D9 = dxp;
    (*ppD3D9)->AddRef();
    return S_OK;
}

HRESULT D3D9ExDevice::GetDeviceCaps(D3DCAPS9* pCaps)
{
    return d3dDevice->GetDeviceCaps(pCaps);
}

HRESULT D3D9ExDevice::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
    return d3dDevice->GetDisplayMode(iSwapChain, pMode);
}

HRESULT D3D9ExDevice::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters)
{
    return d3dDevice->GetCreationParameters(pParameters);
}

HRESULT D3D9ExDevice::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, D3D9ExSurface* pCursorBitmap)
{
    return S_OK;
}

void D3D9ExDevice::SetCursorPosition(int X, int Y, DWORD Flags)
{
    return d3dDevice->SetCursorPosition(X, Y, Flags);
}

BOOL D3D9ExDevice::ShowCursor(BOOL bShow)
{
    return d3dDevice->ShowCursor(bShow);
}

HRESULT D3D9ExDevice::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain)
{
    return d3dDevice->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
}

HRESULT D3D9ExDevice::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain)
{
    return d3dDevice->GetSwapChain(iSwapChain, pSwapChain);
}

UINT D3D9ExDevice::GetNumberOfSwapChains()
{
    return d3dDevice->GetNumberOfSwapChains();
}

HRESULT D3D9ExDevice::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    return S_OK;
}

HRESULT D3D9ExDevice::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
    return d3dDevice->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT D3D9ExDevice::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, D3D9ExSurface** ppBackBuffer)
{
    if (dxpBackBuffer)
    {
        *ppBackBuffer = dxpBackBuffer;
        (*ppBackBuffer)->AddRef();
        return S_OK;
    }

    IDirect3DSurface9* d3dSurface;

    HRESULT result = d3dDevice->GetBackBuffer(iSwapChain, iBackBuffer, Type, &d3dSurface);
    if (FAILED(result))
        return result;

    dxpBackBuffer = new D3D9ExSurface(this, d3dSurface);

    *ppBackBuffer = dxpBackBuffer;
    (*ppBackBuffer)->AddRef();

    d3dSurface->Release();
    return result;
}

HRESULT D3D9ExDevice::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
    return d3dDevice->GetRasterStatus(iSwapChain, pRasterStatus);
}

HRESULT D3D9ExDevice::SetDialogBoxMode(BOOL bEnableDialogs)
{
    return d3dDevice->SetDialogBoxMode(bEnableDialogs);
}

void D3D9ExDevice::SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)
{
    return d3dDevice->SetGammaRamp(iSwapChain, Flags, pRamp);
}

void D3D9ExDevice::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp)
{
    return d3dDevice->GetGammaRamp(iSwapChain, pRamp);
}

HRESULT D3D9ExDevice::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, D3D9ExTexture** ppTexture, HANDLE* pSharedHandle)
{
    IDirect3DTexture9* d3dTexture = nullptr;

    if (Usage == D3DUSAGE_RENDERTARGET && Configuration::enable10BitOutput)
    {
        switch (Format)
        {
        case D3DFMT_A8B8G8R8:
        case D3DFMT_A8R8G8B8:
            Format = D3DFMT_A16B16G16R16;
            break;

        case D3DFMT_L8:
            Format = D3DFMT_L16;
            break;

        default:
            break;
        }
    }

    HRESULT result = d3dDevice->CreateTexture(Width, Height, Levels, Usage | (Pool == D3DPOOL_MANAGED ? D3DUSAGE_DYNAMIC : NULL), Format, Pool == D3DPOOL_MANAGED ? D3DPOOL_DEFAULT : Pool, &d3dTexture, pSharedHandle);
    if (FAILED(result))
        return result;

    *ppTexture = new D3D9ExTexture(this, d3dTexture);

    d3dTexture->Release();
    return result;
}

HRESULT D3D9ExDevice::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
    return d3dDevice->CreateVolumeTexture(Width, Height, Depth, Levels, Usage | (Pool == D3DPOOL_MANAGED ? D3DUSAGE_DYNAMIC : NULL), Format, Pool == D3DPOOL_MANAGED ? D3DPOOL_DEFAULT : Pool, ppVolumeTexture, pSharedHandle);
}

HRESULT D3D9ExDevice::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, D3D9ExCubeTexture** ppCubeTexture, HANDLE* pSharedHandle)
{
    IDirect3DCubeTexture9* d3dCubeTexture;

    HRESULT result = d3dDevice->CreateCubeTexture(EdgeLength, Levels, Usage | (Pool == D3DPOOL_MANAGED ? D3DUSAGE_DYNAMIC : NULL), Format, Pool == D3DPOOL_MANAGED ? D3DPOOL_DEFAULT : Pool, &d3dCubeTexture, pSharedHandle);
    if (FAILED(result))
        return result;

    *ppCubeTexture = new D3D9ExCubeTexture(this, d3dCubeTexture);

    d3dCubeTexture->Release();
    return result;
}

HRESULT D3D9ExDevice::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, D3D9ExVertexBuffer** ppVertexBuffer, HANDLE* pSharedHandle)
{
    IDirect3DVertexBuffer9* d3dVertexBuffer;

    HRESULT result = d3dDevice->CreateVertexBuffer(Length, Usage, FVF, D3DPOOL_DEFAULT, &d3dVertexBuffer, pSharedHandle);
    if (FAILED(result))
        return result;

    *ppVertexBuffer = new D3D9ExVertexBuffer(this, d3dVertexBuffer);

    d3dVertexBuffer->Release();
    return result;
}

HRESULT D3D9ExDevice::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, D3D9ExIndexBuffer** ppIndexBuffer, HANDLE* pSharedHandle)
{
    IDirect3DIndexBuffer9* d3dIndexBuffer;

    HRESULT result = d3dDevice->CreateIndexBuffer(Length, Usage, Format, D3DPOOL_DEFAULT, &d3dIndexBuffer, pSharedHandle);
    if (FAILED(result))
        return result;

    *ppIndexBuffer = new D3D9ExIndexBuffer(this, d3dIndexBuffer);

    d3dIndexBuffer->Release();
    return result;
}

HRESULT D3D9ExDevice::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, D3D9ExSurface** ppSurface, HANDLE* pSharedHandle)
{
    IDirect3DSurface9* d3dSurface;

    HRESULT result = d3dDevice->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, &d3dSurface, pSharedHandle);
    if (FAILED(result))
        return result;

    *ppSurface = new D3D9ExSurface(this, d3dSurface);

    d3dSurface->Release();
    return result;
}

HRESULT D3D9ExDevice::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, D3D9ExSurface** ppSurface, HANDLE* pSharedHandle)
{
    IDirect3DSurface9* d3dSurface;

    HRESULT result = d3dDevice->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, &d3dSurface, pSharedHandle);
    if (FAILED(result))
        return result;

    *ppSurface = new D3D9ExSurface(this, d3dSurface);

    d3dSurface->Release();
    return result;
}

HRESULT D3D9ExDevice::UpdateSurface(D3D9ExSurface* pSourceSurface, CONST RECT* pSourceRect, D3D9ExSurface* pDestinationSurface, CONST POINT* pDestPoint)
{
    return d3dDevice->UpdateSurface(pSourceSurface->GetD3DSurface(), pSourceRect, pDestinationSurface->GetD3DSurface(), pDestPoint);
}

HRESULT D3D9ExDevice::UpdateTexture(D3D9ExBaseTexture* pSourceTexture, D3D9ExBaseTexture* pDestinationTexture)
{
    return d3dDevice->UpdateTexture(pSourceTexture->GetD3DBaseTexture(), pDestinationTexture->GetD3DBaseTexture());
}

HRESULT D3D9ExDevice::GetRenderTargetData(D3D9ExSurface* pRenderTarget, D3D9ExSurface* pDestSurface)
{
    return d3dDevice->GetRenderTargetData(pRenderTarget->GetD3DSurface(), pDestSurface->GetD3DSurface());
}

HRESULT D3D9ExDevice::GetFrontBufferData(UINT iSwapChain, D3D9ExSurface* pDestSurface)
{
    return d3dDevice->GetFrontBufferData(iSwapChain, pDestSurface->GetD3DSurface());
}

HRESULT D3D9ExDevice::StretchRect(D3D9ExSurface* pSourceSurface, CONST RECT* pSourceRect, D3D9ExSurface* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
    return d3dDevice->StretchRect(pSourceSurface->GetD3DSurface(), pSourceRect, pDestSurface->GetD3DSurface(), pDestRect, Filter);
}

HRESULT D3D9ExDevice::ColorFill(D3D9ExSurface* pSurface, CONST RECT* pRect, D3DCOLOR color)
{
    return d3dDevice->ColorFill(pSurface->GetD3DSurface(), pRect, color);
}

HRESULT D3D9ExDevice::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, D3D9ExSurface** ppSurface, HANDLE* pSharedHandle)
{
    IDirect3DSurface9* d3dSurface;

    HRESULT result = d3dDevice->CreateOffscreenPlainSurface(Width, Height, Format, Pool, &d3dSurface, pSharedHandle);
    if (FAILED(result))
        return result;

    *ppSurface = new D3D9ExSurface(this, d3dSurface);

    d3dSurface->Release();
    return result;
}

HRESULT D3D9ExDevice::SetRenderTarget(DWORD RenderTargetIndex, D3D9ExSurface* pRenderTarget)
{
    if (dxpRenderTargets[RenderTargetIndex] == pRenderTarget) return S_OK;

    IDirect3DSurface9* d3dRenderTarget = pRenderTarget ? pRenderTarget->GetD3DSurface() : nullptr;

    HRESULT result = d3dDevice->SetRenderTarget(RenderTargetIndex, d3dRenderTarget);
    if (SUCCEEDED(result))
    {
        if (dxpRenderTargets[RenderTargetIndex])
            dxpRenderTargets[RenderTargetIndex]->Release();

        dxpRenderTargets[RenderTargetIndex] = pRenderTarget;

        if (dxpRenderTargets[RenderTargetIndex])
            dxpRenderTargets[RenderTargetIndex]->AddRef();
    }

    return result;
}

HRESULT D3D9ExDevice::GetRenderTarget(DWORD RenderTargetIndex, D3D9ExSurface** ppRenderTarget)
{
    *ppRenderTarget = dxpRenderTargets[RenderTargetIndex];
    if (*ppRenderTarget)
        (*ppRenderTarget)->AddRef();

    return S_OK;
}

HRESULT D3D9ExDevice::SetDepthStencilSurface(D3D9ExSurface* pNewZStencil)
{
    if (dxpDepthStencil == pNewZStencil) return S_OK;

    IDirect3DSurface9* d3dNewZStencil = pNewZStencil ? pNewZStencil->GetD3DSurface() : nullptr;

    HRESULT result = d3dDevice->SetDepthStencilSurface(d3dNewZStencil);
    if (SUCCEEDED(result))
    {
        if (dxpDepthStencil)
            dxpDepthStencil->Release();

        dxpDepthStencil = pNewZStencil;

        if (dxpDepthStencil)
            dxpDepthStencil->AddRef();
    }

    return result;
}

HRESULT D3D9ExDevice::GetDepthStencilSurface(D3D9ExSurface** ppZStencilSurface)
{
    *ppZStencilSurface = dxpDepthStencil;
    if (*ppZStencilSurface)
        (*ppZStencilSurface)->AddRef();

    return S_OK;
}

HRESULT D3D9ExDevice::BeginScene()
{
    return d3dDevice->BeginScene();
}

HRESULT D3D9ExDevice::EndScene()
{
    return d3dDevice->EndScene();
}

HRESULT D3D9ExDevice::Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
    return d3dDevice->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT D3D9ExDevice::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
    return d3dDevice->SetTransform(State, pMatrix);
}

HRESULT D3D9ExDevice::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
    return d3dDevice->GetTransform(State, pMatrix);
}

HRESULT D3D9ExDevice::MultiplyTransform(D3DTRANSFORMSTATETYPE arg0, CONST D3DMATRIX* arg1)
{
    return d3dDevice->MultiplyTransform(arg0, arg1);
}

HRESULT D3D9ExDevice::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
    return d3dDevice->SetViewport(pViewport);
}

HRESULT D3D9ExDevice::GetViewport(D3DVIEWPORT9* pViewport)
{
    return d3dDevice->GetViewport(pViewport);
}

HRESULT D3D9ExDevice::SetMaterial(CONST D3DMATERIAL9* pMaterial)
{
    return d3dDevice->SetMaterial(pMaterial);
}

HRESULT D3D9ExDevice::GetMaterial(D3DMATERIAL9* pMaterial)
{
    return d3dDevice->GetMaterial(pMaterial);
}

HRESULT D3D9ExDevice::SetLight(DWORD Index, CONST D3DLIGHT9* arg1)
{
    return d3dDevice->SetLight(Index, arg1);
}

HRESULT D3D9ExDevice::GetLight(DWORD Index, D3DLIGHT9* arg1)
{
    return d3dDevice->GetLight(Index, arg1);
}

HRESULT D3D9ExDevice::LightEnable(DWORD Index, BOOL Enable)
{
    return d3dDevice->LightEnable(Index, Enable);
}

HRESULT D3D9ExDevice::GetLightEnable(DWORD Index, BOOL* pEnable)
{
    return d3dDevice->GetLightEnable(Index, pEnable);
}

HRESULT D3D9ExDevice::SetClipPlane(DWORD Index, CONST float* pPlane)
{
    return d3dDevice->SetClipPlane(Index, pPlane);
}

HRESULT D3D9ExDevice::GetClipPlane(DWORD Index, float* pPlane)
{
    return d3dDevice->GetClipPlane(Index, pPlane);
}

HRESULT D3D9ExDevice::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
    return d3dDevice->SetRenderState(State, Value);
}

HRESULT D3D9ExDevice::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue)
{
    return d3dDevice->GetRenderState(State, pValue);
}

HRESULT D3D9ExDevice::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
    return d3dDevice->CreateStateBlock(Type, ppSB);
}

HRESULT D3D9ExDevice::BeginStateBlock()
{
    return d3dDevice->BeginStateBlock();
}

HRESULT D3D9ExDevice::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
    return d3dDevice->EndStateBlock(ppSB);
}

HRESULT D3D9ExDevice::SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus)
{
    return d3dDevice->SetClipStatus(pClipStatus);
}

HRESULT D3D9ExDevice::GetClipStatus(D3DCLIPSTATUS9* pClipStatus)
{
    return d3dDevice->GetClipStatus(pClipStatus);
}

HRESULT D3D9ExDevice::GetTexture(DWORD Stage, D3D9ExBaseTexture** ppTexture)
{
    *ppTexture = dxpStageTextures[Stage];
    if (*ppTexture)
        (*ppTexture)->AddRef();

    return S_OK;
}

HRESULT D3D9ExDevice::SetTexture(DWORD Stage, D3D9ExBaseTexture* pTexture)
{
    if (dxpStageTextures[Stage] == pTexture) return S_OK;

    IDirect3DBaseTexture9* d3dTexture = pTexture ? pTexture->GetD3DBaseTexture() : nullptr;

    HRESULT result = d3dDevice->SetTexture(Stage, d3dTexture);
    if (SUCCEEDED(result))
    {
        if (dxpStageTextures[Stage])
            dxpStageTextures[Stage]->Release();

        dxpStageTextures[Stage] = pTexture;

        if (dxpStageTextures[Stage])
            dxpStageTextures[Stage]->AddRef();
    }

    return result;
}

HRESULT D3D9ExDevice::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
    return d3dDevice->GetTextureStageState(Stage, Type, pValue);
}

HRESULT D3D9ExDevice::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
    return d3dDevice->SetTextureStageState(Stage, Type, Value);
}

HRESULT D3D9ExDevice::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
    return d3dDevice->GetSamplerState(Sampler, Type, pValue);
}

HRESULT D3D9ExDevice::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
    return d3dDevice->SetSamplerState(Sampler, Type, Value);
}

HRESULT D3D9ExDevice::ValidateDevice(DWORD* pNumPasses)
{
    return d3dDevice->ValidateDevice(pNumPasses);
}

HRESULT D3D9ExDevice::SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries)
{
    return d3dDevice->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT D3D9ExDevice::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries)
{
    return d3dDevice->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT D3D9ExDevice::SetCurrentTexturePalette(UINT PaletteNumber)
{
    return d3dDevice->SetCurrentTexturePalette(PaletteNumber);
}

HRESULT D3D9ExDevice::GetCurrentTexturePalette(UINT* PaletteNumber)
{
    return d3dDevice->GetCurrentTexturePalette(PaletteNumber);
}

HRESULT D3D9ExDevice::SetScissorRect(CONST RECT* pRect)
{
    return d3dDevice->SetScissorRect(pRect);
}

HRESULT D3D9ExDevice::GetScissorRect(RECT* pRect)
{
    return d3dDevice->GetScissorRect(pRect);
}

HRESULT D3D9ExDevice::SetSoftwareVertexProcessing(BOOL bSoftware)
{
    return d3dDevice->SetSoftwareVertexProcessing(bSoftware);
}

BOOL D3D9ExDevice::GetSoftwareVertexProcessing()
{
    return d3dDevice->GetSoftwareVertexProcessing();
}

HRESULT D3D9ExDevice::SetNPatchMode(float nSegments)
{
    return d3dDevice->SetNPatchMode(nSegments);
}

float D3D9ExDevice::GetNPatchMode()
{
    return d3dDevice->GetNPatchMode();
}

HRESULT D3D9ExDevice::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
    return d3dDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT D3D9ExDevice::DrawIndexedPrimitive(D3DPRIMITIVETYPE arg0, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
    return d3dDevice->DrawIndexedPrimitive(arg0, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

HRESULT D3D9ExDevice::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    return d3dDevice->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT D3D9ExDevice::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    return d3dDevice->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT D3D9ExDevice::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, D3D9ExVertexBuffer* pDestBuffer, D3D9ExVertexDeclaration* pVertexDecl, DWORD Flags)
{
    return d3dDevice->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer->GetD3DVertexBuffer(), pVertexDecl->GetD3DVertexDeclaration(), Flags);
}

HRESULT D3D9ExDevice::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, D3D9ExVertexDeclaration** ppDecl)
{
    IDirect3DVertexDeclaration9* d3dDecl;

    HRESULT result = d3dDevice->CreateVertexDeclaration(pVertexElements, &d3dDecl);
    if (FAILED(result))
        return result;

    *ppDecl = new D3D9ExVertexDeclaration(this, d3dDecl);

    d3dDecl->Release();
    return result;
}

HRESULT D3D9ExDevice::SetVertexDeclaration(D3D9ExVertexDeclaration* pDecl)
{
    if (dxpVertexDeclaration == pDecl) return S_OK;

    IDirect3DVertexDeclaration9* d3dDecl = pDecl ? pDecl->GetD3DVertexDeclaration() : nullptr;

    HRESULT result = d3dDevice->SetVertexDeclaration(d3dDecl);
    if (SUCCEEDED(result))
    {
        if (dxpVertexDeclaration)
            dxpVertexDeclaration->Release();

        dxpVertexDeclaration = pDecl;

        if (dxpVertexDeclaration)
            dxpVertexDeclaration->AddRef();
    }

    return result;
}

HRESULT D3D9ExDevice::GetVertexDeclaration(D3D9ExVertexDeclaration** ppDecl)
{
    *ppDecl = dxpVertexDeclaration;
    if (*ppDecl)
        (*ppDecl)->AddRef();

    return S_OK;
}

HRESULT D3D9ExDevice::SetFVF(DWORD FVF)
{
    return d3dDevice->SetFVF(FVF);
}

HRESULT D3D9ExDevice::GetFVF(DWORD* pFVF)
{
    return d3dDevice->GetFVF(pFVF);
}

HRESULT D3D9ExDevice::CreateVertexShader(CONST DWORD* pFunction, D3D9ExVertexShader** ppShader, DWORD FunctionSize)
{
    IDirect3DVertexShader9* d3dVertexShader;

    HRESULT result = d3dDevice->CreateVertexShader(pFunction, &d3dVertexShader);
    if (FAILED(result))
        return result;

    *ppShader = new D3D9ExVertexShader(this, d3dVertexShader);

    d3dVertexShader->Release();
    return result;
}

HRESULT D3D9ExDevice::SetVertexShader(D3D9ExVertexShader* pShader)
{
    if (dxpVertexShader == pShader) return S_OK;

    IDirect3DVertexShader9* d3dShader = pShader ? pShader->GetD3DVertexShader() : nullptr;

    HRESULT result = d3dDevice->SetVertexShader(d3dShader);
    if (SUCCEEDED(result))
    {
        if (dxpVertexShader)
            dxpVertexShader->Release();

        dxpVertexShader = pShader;

        if (dxpVertexShader)
            dxpVertexShader->AddRef();
    }

    return result;
}

HRESULT D3D9ExDevice::GetVertexShader(D3D9ExVertexShader** ppShader)
{
    *ppShader = dxpVertexShader;
    if (*ppShader)
        (*ppShader)->AddRef();

    return S_OK;
}

HRESULT D3D9ExDevice::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    return d3dDevice->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT D3D9ExDevice::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
    return d3dDevice->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT D3D9ExDevice::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    return d3dDevice->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT D3D9ExDevice::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
    return d3dDevice->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT D3D9ExDevice::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
    void* pDst = &boolParamsVs[StartRegister];
    const size_t dstSize = BoolCount * sizeof(BOOL);

    if (memcmp(pDst, pConstantData, dstSize) == 0) return S_OK;
    memcpy(pDst, pConstantData, dstSize);

    return d3dDevice->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT D3D9ExDevice::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
    return d3dDevice->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT D3D9ExDevice::SetStreamSource(UINT StreamNumber, D3D9ExVertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride)
{
    StreamSource& src = streamSources[StreamNumber];
    if (src.dxpVertexBuffer == pStreamData && src.OffsetInBytes == OffsetInBytes && src.Stride == Stride)
        return S_OK;

    IDirect3DVertexBuffer9* d3dStreamData = pStreamData ? pStreamData->GetD3DVertexBuffer() : nullptr;

    HRESULT result = d3dDevice->SetStreamSource(StreamNumber, d3dStreamData, OffsetInBytes, Stride);;
    if (SUCCEEDED(result))
    {
        if (src.dxpVertexBuffer)
            src.dxpVertexBuffer->Release();

        src.dxpVertexBuffer = pStreamData;

        if (src.dxpVertexBuffer)
            src.dxpVertexBuffer->AddRef();
    }

    return result;
}

HRESULT D3D9ExDevice::GetStreamSource(UINT StreamNumber, D3D9ExVertexBuffer** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)
{
    // ??? How do I handle this properly??
    return S_OK;
}

HRESULT D3D9ExDevice::SetStreamSourceFreq(UINT StreamNumber, UINT Setting)
{
    return d3dDevice->SetStreamSourceFreq(StreamNumber, Setting);
}

HRESULT D3D9ExDevice::GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting)
{
    return d3dDevice->GetStreamSourceFreq(StreamNumber, pSetting);
}

HRESULT D3D9ExDevice::SetIndices(D3D9ExIndexBuffer* pIndexData)
{
    if (dxpIndexBuffer == pIndexData) return S_OK;

    IDirect3DIndexBuffer9* d3dIndexData = pIndexData ? pIndexData->GetD3DIndexBuffer() : nullptr;

    HRESULT result = d3dDevice->SetIndices(d3dIndexData);
    if (SUCCEEDED(result))
    {
        if (dxpIndexBuffer)
            dxpIndexBuffer->Release();

        dxpIndexBuffer = pIndexData;

        if (dxpIndexBuffer)
            dxpIndexBuffer->AddRef();
    }

    return result;
}

HRESULT D3D9ExDevice::GetIndices(D3D9ExIndexBuffer** ppIndexData)
{
    *ppIndexData = dxpIndexBuffer;
    if (*ppIndexData)
        (*ppIndexData)->AddRef();

    return S_OK;
}

HRESULT D3D9ExDevice::CreatePixelShader(CONST DWORD* pFunction, D3D9ExPixelShader** ppShader, DWORD FunctionSize)
{
    IDirect3DPixelShader9* d3dPixelShader;

    HRESULT result = d3dDevice->CreatePixelShader(pFunction, &d3dPixelShader);
    if (FAILED(result))
        return result;

    *ppShader = new D3D9ExPixelShader(this, d3dPixelShader);

    d3dPixelShader->Release();
    return result;
}

HRESULT D3D9ExDevice::SetPixelShader(D3D9ExPixelShader* pShader)
{
    if (dxpPixelShader == pShader) return S_OK;

    IDirect3DPixelShader9* d3dShader = pShader ? pShader->GetD3DPixelShader() : nullptr;

    HRESULT result = d3dDevice->SetPixelShader(d3dShader);
    if (SUCCEEDED(result))
    {
        if (dxpPixelShader)
            dxpPixelShader->Release();

        dxpPixelShader = pShader;

        if (dxpPixelShader)
            dxpPixelShader->AddRef();
    }

    return result;
}

HRESULT D3D9ExDevice::GetPixelShader(D3D9ExPixelShader** ppShader)
{
    *ppShader = dxpPixelShader;
    if (*ppShader)
        (*ppShader)->AddRef();

    return S_OK;
}

HRESULT D3D9ExDevice::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    return d3dDevice->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT D3D9ExDevice::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
    return d3dDevice->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT D3D9ExDevice::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    return d3dDevice->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT D3D9ExDevice::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
    return d3dDevice->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT D3D9ExDevice::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
    void* pDst = &boolParamsPs[StartRegister];
    const size_t dstSize = BoolCount * sizeof(BOOL);

    if (memcmp(pDst, pConstantData, dstSize) == 0) return S_OK;
    memcpy(pDst, pConstantData, dstSize);

    return d3dDevice->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT D3D9ExDevice::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
    return d3dDevice->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT D3D9ExDevice::DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
    return d3dDevice->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

HRESULT D3D9ExDevice::DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
    return d3dDevice->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

HRESULT D3D9ExDevice::DeletePatch(UINT Handle)
{
    return d3dDevice->DeletePatch(Handle);
}

HRESULT D3D9ExDevice::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
    return d3dDevice->CreateQuery(Type, ppQuery);
}