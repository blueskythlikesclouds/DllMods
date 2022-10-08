#include "Device.h"

#include "Buffer.h"
#include "DepthStencilSurface.h"
#include "DepthStencilTexture.h"
#include "FVF.wvu.h"
#include "PixelShader.h"
#include "RenderTargetSurface.h"
#include "RenderTargetTexture.h"
#include "ShaderCache.h"
#include "Surface.h"
#include "SwapChainDefault.h"
#include "SwapChainWaitable.h"
#include "Texture.h"
#include "TypeConverter.h"
#include "VertexDeclaration.h"
#include "VertexShader.h"

#define LOCK_GUARD() std::lock_guard lock(criticalSection)

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

alignas(64) GlobalsVS globalsVS;
alignas(64) GlobalsPS globalsPS;
alignas(64) GlobalsShared globalsShared;

void Device::flush()
{
    if (dirty & (1 << DirtyRenderTarget))
    {
        ID3D11RenderTargetView* renderTargetViews[_countof(renderTargets)];

        size_t i;
        for (i = 0; i < _countof(renderTargets) && renderTargets[i]; i++)
            renderTargetViews[i] = renderTargets[i]->getRTV();

        deviceContext->OMSetRenderTargets(i, renderTargetViews, depthStencil ? depthStencil->getDSV() : nullptr);
    }

    if (dirty & (1 << DirtyViewport))
        deviceContext->RSSetViewports(1, &viewport);

    if (dirty & (1 << DirtyDepthStencilState))
    {
        auto& ptr = depthStencilStates[XXH32(&depthStencilState, sizeof(depthStencilState), 0)];
        if (!ptr)
            device->CreateDepthStencilState(&depthStencilState, ptr.GetAddressOf());

        deviceContext->OMSetDepthStencilState(ptr.Get(), 0);
    }

    if (dirty & (1 << DirtyRasterizerState))
    {
        auto& ptr = rasterizerStates[XXH32(&rasterizerState, sizeof(rasterizerState), 0)];
        if (!ptr)
            device->CreateRasterizerState(&rasterizerState, ptr.GetAddressOf());

        deviceContext->RSSetState(ptr.Get());
    }

    if (dirty & (1 << DirtyBlendState))
    {
        auto& ptr = blendStates[XXH32(&blendState, sizeof(blendState), 0)];
        if (!ptr)
            device->CreateBlendState(&blendState, ptr.GetAddressOf());

        const FLOAT blendFactor[4] { 1, 1, 1, 1 };
        deviceContext->OMSetBlendState(ptr.Get(), blendFactor, ~0);
    }

    if (dirty & (1 << DirtyTexture))
    {
        for (size_t i = 0; i < _countof(textures); i++)
        {
            if (!(dirtyTextures & (1 << i)))
                continue;

            ID3D11ShaderResourceView* shaderResourceView = textures[i] ? textures[i]->getSRV() : nullptr;
            deviceContext->PSSetShaderResources(i, 1, &shaderResourceView);
        }
    }

    if (dirty & (1 << DirtySampler))
    {
        for (size_t i = 0; i < _countof(samplers); i++)
        {
            if (!(dirtySamplers & (1 << i)))
                continue;

            auto& ptr = samplerStates[XXH32(&samplers[i], sizeof(samplers[i]), 0)];
            if (!ptr)
                device->CreateSamplerState(&samplers[i], ptr.GetAddressOf());

            deviceContext->PSSetSamplers(i, 1, ptr.GetAddressOf());
        }
    }

    if (dirty & (1 << DirtyScissorRect))
        deviceContext->RSSetScissorRects(1, &scissorRect);

    if (dirty & (1 << DirtyPrimitiveTopology))
        deviceContext->IASetPrimitiveTopology(primitiveTopology);

    if (dirty & (1 << DirtyVertexShader) && vertexDeclaration)
    {
        const VertexShader* shader = !vertexShader || vertexDeclaration->getIsFVF() ? fvfVertexShader.Get() : vertexShader.Get();

        deviceContext->VSSetShader(shader->getVertexShader(), nullptr, 0);
        deviceContext->IASetInputLayout(vertexDeclaration->getInputLayout(device.Get(), shader, enableInstancing));
    }

    if (dirty & (1 << DirtyVertexConstant))
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubResource;
        deviceContext->Map(globalsPSBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
        memcpy(mappedSubResource.pData, &globalsVS, sizeof(globalsVS));
        deviceContext->Unmap(globalsPSBuffer.Get(), 0);
    }

    if (dirty & (1 << DirtyVertexBuffer))
    {
        ID3D11Buffer* buffers[_countof(vertexBuffers)];

        size_t i;
        for (i = 0; i < _countof(vertexBuffers) && vertexBuffers[i]; i++)
            buffers[i] = vertexBuffers[i].Get();

        deviceContext->IASetVertexBuffers(0, i, buffers, vertexStrides, vertexOffsets);
    }

    if (dirty & (1 << DirtyIndexBuffer) && indexBuffer)
        deviceContext->IASetIndexBuffer(reinterpret_cast<ID3D11Buffer*>(indexBuffer->getResource()), indexBuffer->getFormat(), 0);

    if (dirty & (1 << DirtyPixelShader) && pixelShader)
        deviceContext->PSSetShader(pixelShader->getPixelShader(), nullptr, 0);

    if (dirty & (1 << DirtyPixelConstant))
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubResource;
        deviceContext->Map(globalsVSBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
        memcpy(mappedSubResource.pData, &globalsPS, sizeof(globalsPS));
        deviceContext->Unmap(globalsVSBuffer.Get(), 0);
    }

    bool currHasBone = (globalsShared.booleans & mrgHasBone) != 0;
    if (vertexDeclaration && enableInstancing)
        currHasBone &= vertexDeclaration->getHasBone();

    if (dirty & (1 << DirtySharedConstant) || currHasBone != hasBone)
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubResource;
        deviceContext->Map(globalsSharedBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);

        *(UINT*)mappedSubResource.pData = (globalsShared.booleans & ~mrgHasBone) | (currHasBone ? mrgHasBone : 0);
        memcpy((char*)mappedSubResource.pData + offsetof(GlobalsShared, flags), &globalsShared.flags,
               sizeof(GlobalsShared) - offsetof(GlobalsShared, flags));

        deviceContext->Unmap(globalsSharedBuffer.Get(), 0);
    }

    hasBone = currHasBone;

    dirty = 0;
    dirtyTextures = 0;
    dirtySamplers = 0;
}

void Device::setDirty(void* dest, const void* src, const size_t byteSize, const size_t dirtyStateIndex)
{
    if (memcmp(dest, src, byteSize) == 0)
        return;

    memcpy(dest, src, byteSize);
    dirty |= 1 << dirtyStateIndex;
}

void Device::setTexture(Texture* texture, const size_t index)
{
    if (textures[index].Get() == texture)
        return;

    textures[index] = texture;
    dirty |= 1 << DirtyTexture;
    dirtyTextures |= 1 << index;
}

void Device::setSamplerState(void* dest, const void* src, const size_t byteSize, const size_t index)
{
    if (memcmp(dest, src, byteSize) == 0)
        return;

    memcpy(dest, src, byteSize);
    dirty |= 1 << DirtySampler;
    dirtySamplers |= 1 << index;
}

void Device::invalidateDirtyStates()
{
    dirty = ~0;
    dirtyTextures = ~0;
    dirtySamplers = ~0;

    LOCK_GUARD();

    deviceContext->VSSetConstantBuffers(0, 1, globalsPSBuffer.GetAddressOf());
    deviceContext->VSSetConstantBuffers(1, 1, globalsSharedBuffer.GetAddressOf());

    deviceContext->PSSetConstantBuffers(0, 1, globalsVSBuffer.GetAddressOf());
    deviceContext->PSSetConstantBuffers(1, 1, globalsSharedBuffer.GetAddressOf());
}

bool Device::reserveUploadVertexBuffer(const void* data, const size_t size)
{
    if (size <= uploadVertexBufferSize)
        return false;

    D3D11_BUFFER_DESC desc{};
    desc.ByteWidth = size;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA initialData{};
    initialData.pSysMem = data;

    device->CreateBuffer(&desc, &initialData, uploadVertexBuffer.ReleaseAndGetAddressOf());
    uploadVertexBufferSize = size;

    return true;
}

bool Device::shouldSkipDrawing() const
{
    std::lock_guard lock(ShaderCache::criticalSection);
    return (pixelShader && !pixelShader->getPixelShader()) || (vertexShader && !vertexShader->getVertexShader());
}

Device::Device(D3DPRESENT_PARAMETERS* presentationParameters, DXGI_SCALING scaling)
    : syncInterval(presentationParameters->PresentationInterval == 1 ? 1 : 0)
{
    D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
#if _DEBUG
        D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
#else
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
#endif
        nullptr,
        0,
        D3D11_SDK_VERSION,
        device.GetAddressOf(),
        nullptr,
        deviceContext.GetAddressOf());

#if _DEBUG
    ComPtr<ID3D11Debug> debug;
    device.As(&debug);

    ComPtr<ID3D11InfoQueue> infoQueue;
    debug.As(&infoQueue);

    // Disable messages we're aware of and okay with
    D3D11_MESSAGE_ID ids[] =
    {
        D3D11_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD,
        D3D11_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD,
        D3D11_MESSAGE_ID_OMSETRENDERTARGETS_INVALIDVIEW,
        D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET,
        D3D11_MESSAGE_ID_DEVICE_DRAW_SAMPLER_MISMATCH,
    };

    D3D11_INFO_QUEUE_FILTER filter {};
    filter.DenyList.NumIDs = _countof(ids);
    filter.DenyList.pIDList = ids;

    infoQueue->AddStorageFilterEntries(&filter);
#endif

    if (presentationParameters->Windowed)
        swapChain = std::make_unique<SwapChainWaitable>();
    else
        swapChain = std::make_unique<SwapChainDefault>();

    swapChain->initialize(this, presentationParameters, scaling);

    depthStencilState = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
    rasterizerState = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
    blendState = CD3D11_BLEND_DESC(CD3D11_DEFAULT());

    for (auto& sampler : samplers)
    {
        sampler = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
        sampler.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    }

    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = (sizeof(globalsVS) + 15) & ~15;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    device->CreateBuffer(&bufferDesc, nullptr, globalsPSBuffer.GetAddressOf());

    bufferDesc.ByteWidth = (sizeof(globalsPS) + 15) & ~15;
    device->CreateBuffer(&bufferDesc, nullptr, globalsVSBuffer.GetAddressOf());

    bufferDesc.ByteWidth = (sizeof(globalsShared) + 15) & ~15;
    device->CreateBuffer(&bufferDesc, nullptr, globalsSharedBuffer.GetAddressOf());

    fvfVertexShader.Attach(new VertexShader(device.Get(), (void*)g_fvf_vs_main, sizeof(g_fvf_vs_main)));

    invalidateDirtyStates();
}

ID3D11Device* Device::get() const
{
    return device.Get();
}

ID3D11DeviceContext* Device::getContext() const
{
    return deviceContext.Get();
}

CriticalSection& Device::getCriticalSection()
{
    return criticalSection;
}

std::lock_guard<CriticalSection> Device::lock()
{
    return std::lock_guard(criticalSection);
}

FUNCTION_STUB(HRESULT, Device::TestCooperativeLevel)

FUNCTION_STUB(UINT, Device::GetAvailableTextureMem)

FUNCTION_STUB(HRESULT, Device::EvictManagedResources)

FUNCTION_STUB(HRESULT, Device::GetDirect3D, D3D9** ppD3D9)

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
    swapChain->present(this, syncInterval);

    invalidateDirtyStates();
    return S_OK;
}

HRESULT Device::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, Surface** ppBackBuffer)
{
    *ppBackBuffer = swapChain->getRenderTargetSurface();
    (*ppBackBuffer)->AddRef();

    return S_OK;
}
    
FUNCTION_STUB(HRESULT, Device::GetRasterStatus, UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)

FUNCTION_STUB(HRESULT, Device::SetDialogBoxMode, BOOL bEnableDialogs)

FUNCTION_STUB(void, Device::SetGammaRamp, UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)

FUNCTION_STUB(void, Device::GetGammaRamp, UINT iSwapChain, D3DGAMMARAMP* pRamp)

HRESULT Device::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, Texture** ppTexture, HANDLE* pSharedHandle)
{
    const DXGI_FORMAT format = TypeConverter::convert(Format);

    if (format == DXGI_FORMAT_UNKNOWN)
    {
        *ppTexture = nullptr;
        return E_FAIL;
    }

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = Width;
    desc.Height = Height;
    desc.MipLevels = Levels;
    desc.ArraySize = 1;
    desc.Format = Usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL) ? TypeConverter::makeTypeless(format) : format;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags =
        Usage & D3DUSAGE_RENDERTARGET ? D3D11_BIND_RENDER_TARGET :
        Usage & D3DUSAGE_DEPTHSTENCIL ? D3D11_BIND_DEPTH_STENCIL : 0;

    // Default usage textures are guaranteed to be modified on CPU
    // as we load DDS textures using DDSTextureLoader11
    if (!Usage)
    {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }

    desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

    ComPtr<ID3D11Texture2D> texture;

    const HRESULT hr = device->CreateTexture2D(&desc, nullptr, texture.GetAddressOf());
        
    if (FAILED(hr))
        return hr;

    if (Usage & D3DUSAGE_RENDERTARGET)
        *ppTexture = new RenderTargetTexture(this, texture.Get(), format);

    else if (Usage & D3DUSAGE_DEPTHSTENCIL)
        *ppTexture = new DepthStencilTexture(this, texture.Get(), format);

    else
        *ppTexture = new Texture(this, texture.Get(), format);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::CreateVolumeTexture, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)

FUNCTION_STUB(HRESULT, Device::CreateCubeTexture, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, CubeTexture** ppCubeTexture, HANDLE* pSharedHandle)

HRESULT Device::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, VertexBuffer** ppVertexBuffer, HANDLE* pSharedHandle)
{
    *ppVertexBuffer = new VertexBuffer(this, Length, D3D11_BIND_VERTEX_BUFFER);
    return S_OK;
}

HRESULT Device::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IndexBuffer** ppIndexBuffer, HANDLE* pSharedHandle)
{
    *ppIndexBuffer = new IndexBuffer(this, Length, D3D11_BIND_INDEX_BUFFER, TypeConverter::convert(Format));
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

HRESULT Device::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, Surface** ppSurface, HANDLE* pSharedHandle)
{
    *ppSurface = nullptr;
    return E_FAIL;
}

HRESULT Device::SetRenderTarget(DWORD RenderTargetIndex, Surface* pRenderTarget)
{
    setDirty(renderTargets[RenderTargetIndex], reinterpret_cast<RenderTargetSurface*>(pRenderTarget), DirtyRenderTarget);
    return S_OK;
}

HRESULT Device::GetRenderTarget(DWORD RenderTargetIndex, Surface** ppRenderTarget)
{
    return S_OK;
}

HRESULT Device::SetDepthStencilSurface(Surface* pNewZStencil)
{
    setDirty(depthStencil, reinterpret_cast<DepthStencilSurface*>(pNewZStencil), DirtyRenderTarget);
    return S_OK;
}

HRESULT Device::GetDepthStencilSurface(Surface** ppZStencilSurface)
{
    *ppZStencilSurface = depthStencil.Get();

    if (depthStencil)
        depthStencil->AddRef();

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
    LOCK_GUARD();
        
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
                deviceContext->ClearRenderTargetView(renderTarget->getRTV(), color);
        }
    }

    if (Flags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL) && depthStencil)
    {
        UINT clearFlag = 0;

        if (Flags & D3DCLEAR_ZBUFFER)
            clearFlag |= D3D11_CLEAR_DEPTH;

        if (Flags & D3DCLEAR_STENCIL)
            clearFlag |= D3D11_CLEAR_STENCIL;

        deviceContext->ClearDepthStencilView(depthStencil->getDSV(), clearFlag, Z, (UINT8)Stencil);
    }

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::SetTransform, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)

FUNCTION_STUB(HRESULT, Device::GetTransform, D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)

FUNCTION_STUB(HRESULT, Device::MultiplyTransform, D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*)

HRESULT Device::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
    D3D11_VIEWPORT newViewport;
    newViewport.TopLeftX = (float)pViewport->X;
    newViewport.TopLeftY = (float)pViewport->Y;
    newViewport.Width = (float)pViewport->Width;
    newViewport.Height = (float)pViewport->Height;
    newViewport.MinDepth = pViewport->MinZ;
    newViewport.MaxDepth = pViewport->MaxZ;

    setDirty(&viewport, &newViewport, sizeof(D3D11_VIEWPORT), DirtyViewport);
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
        setDirty(depthStencilState.DepthEnable, (BOOL)Value, DirtyDepthStencilState);
        break;

    case D3DRS_FILLMODE:
        setDirty(rasterizerState.FillMode, (D3D11_FILL_MODE)Value, DirtyRasterizerState);
        break;

    case D3DRS_ZWRITEENABLE:
        setDirty(depthStencilState.DepthWriteMask, (D3D11_DEPTH_WRITE_MASK)Value, DirtyDepthStencilState);
        break;

    case D3DRS_ALPHATESTENABLE:
        setDirty(globalsShared.flags, globalsShared.flags & ~SHARED_FLAGS_ENABLE_ALPHA_TEST | (Value ? SHARED_FLAGS_ENABLE_ALPHA_TEST : 0), DirtySharedConstant);
        break;

    case D3DRS_SRCBLEND:
        setDirty(blendState.RenderTarget[0].SrcBlend, (D3D11_BLEND)Value, DirtyBlendState);
        break;

    case D3DRS_DESTBLEND:
        setDirty(blendState.RenderTarget[0].DestBlend, (D3D11_BLEND)Value, DirtyBlendState);
        break;

    case D3DRS_CULLMODE:
        setDirty(rasterizerState.CullMode, (D3D11_CULL_MODE)Value, DirtyRasterizerState);
        break;

    case D3DRS_ZFUNC:
        setDirty(depthStencilState.DepthFunc, (D3D11_COMPARISON_FUNC)Value, DirtyDepthStencilState);
        break;

    case D3DRS_ALPHAREF:
        setDirty(globalsShared.alphaThreshold, (float)Value / 255.0f, DirtySharedConstant);
        break;

    case D3DRS_ALPHABLENDENABLE:
        setDirty(blendState.RenderTarget[0].BlendEnable, (BOOL)Value, DirtyBlendState);
        break;

    case D3DRS_COLORWRITEENABLE:
        setDirty(blendState.RenderTarget[0].RenderTargetWriteMask, (UINT8)Value, DirtyBlendState);
        break;

    case D3DRS_BLENDOP:
        setDirty(blendState.RenderTarget[0].BlendOp, (D3D11_BLEND_OP)Value, DirtyBlendState);
        break;

    case D3DRS_SLOPESCALEDEPTHBIAS:
        setDirty(rasterizerState.SlopeScaledDepthBias, *(float*)&Value, DirtyRasterizerState);
        break;

    case D3DRS_COLORWRITEENABLE1:
        setDirty(blendState.RenderTarget[1].RenderTargetWriteMask, (UINT8)Value, DirtyBlendState);
        break;

    case D3DRS_COLORWRITEENABLE2:
        setDirty(blendState.RenderTarget[2].RenderTargetWriteMask, (UINT8)Value, DirtyBlendState);
        break;

    case D3DRS_COLORWRITEENABLE3: 
        setDirty(blendState.RenderTarget[3].RenderTargetWriteMask, (UINT8)Value, DirtyBlendState);
        break;

    case D3DRS_DEPTHBIAS:
        setDirty(rasterizerState.DepthBias, (INT)(*(float*)&Value * (1 << 24)), DirtyRasterizerState);
        break;

    case D3DRS_SRCBLENDALPHA:
        setDirty(blendState.RenderTarget[0].SrcBlendAlpha, (D3D11_BLEND)Value, DirtyBlendState);
        break;

    case D3DRS_DESTBLENDALPHA:
        setDirty(blendState.RenderTarget[0].DestBlendAlpha, (D3D11_BLEND)Value, DirtyBlendState);
        break;

    case D3DRS_BLENDOPALPHA:
        setDirty(blendState.RenderTarget[0].BlendOpAlpha, (D3D11_BLEND_OP)Value, DirtyBlendState);
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
    setTexture(reinterpret_cast<Texture*>(pTexture), Stage);

    // g_VerticalShadowMapSampler/g_ShadowMapSampler
    if (Stage == 7 || Stage == 13)
    {
        size_t filter = samplers[Stage].Filter & ~0x80;
        
        if (textures[Stage] && textures[Stage]->getFormat() == DXGI_FORMAT_D24_UNORM_S8_UINT)
            filter |= 0x80; // Comparison state

        setSamplerState(samplers[Stage].Filter, (D3D11_FILTER)filter, Stage);
    }

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
        setSamplerState(samplers[Sampler].AddressU, (D3D11_TEXTURE_ADDRESS_MODE)Value, Sampler);
        break;

    case D3DSAMP_ADDRESSV:
        setSamplerState(samplers[Sampler].AddressV, (D3D11_TEXTURE_ADDRESS_MODE)Value, Sampler);
        break;

    case D3DSAMP_ADDRESSW:
        setSamplerState(samplers[Sampler].AddressW, (D3D11_TEXTURE_ADDRESS_MODE)Value, Sampler);
        break;

    case D3DSAMP_BORDERCOLOR:
        FLOAT color[4];
        color[2] = (Value & 0xFF) / 255.0f;
        color[1] = ((Value >> 8) & 0xFF) / 255.0f;
        color[0] = ((Value >> 16) & 0xFF) / 255.0f;
        color[3] = ((Value >> 24) & 0xFF) / 255.0f;

        setSamplerState(samplers[Sampler].BorderColor, color, sizeof(samplers[Sampler].BorderColor), Sampler);
        break;

    case D3DSAMP_MAGFILTER:
        if (Value == D3DTEXF_LINEAR)
            setSamplerState(samplers[Sampler].Filter, (D3D11_FILTER)(samplers[Sampler].Filter | 0x4), Sampler);
        else
            setSamplerState(samplers[Sampler].Filter, (D3D11_FILTER)(samplers[Sampler].Filter & ~0x4), Sampler);

        break;

    case D3DSAMP_MINFILTER:
        if (Value == D3DTEXF_LINEAR)
            setSamplerState(samplers[Sampler].Filter, (D3D11_FILTER)(samplers[Sampler].Filter | 0x10), Sampler);
        else
            setSamplerState(samplers[Sampler].Filter, (D3D11_FILTER)(samplers[Sampler].Filter & ~0x10), Sampler);

        break;

    case D3DSAMP_MIPFILTER:
        if (Value == D3DTEXF_LINEAR)
            setSamplerState(samplers[Sampler].Filter, (D3D11_FILTER)(samplers[Sampler].Filter | 0x1), Sampler);
        else
            setSamplerState(samplers[Sampler].Filter, (D3D11_FILTER)(samplers[Sampler].Filter & ~0x1), Sampler);

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
    D3D11_RECT newScissorRect;
    newScissorRect.left = pRect->left;
    newScissorRect.top = pRect->top;
    newScissorRect.right = pRect->right;
    newScissorRect.bottom = pRect->bottom;

    setDirty(&scissorRect, &newScissorRect, sizeof(D3D11_RECT), DirtyScissorRect);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetScissorRect, RECT* pRect)

FUNCTION_STUB(HRESULT, Device::SetSoftwareVertexProcessing, BOOL bSoftware)

FUNCTION_STUB(BOOL, Device::GetSoftwareVertexProcessing)

FUNCTION_STUB(HRESULT, Device::SetNPatchMode, float nSegments)

FUNCTION_STUB(float, Device::GetNPatchMode)

HRESULT Device::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
    if (PrimitiveType > D3DPT_TRIANGLESTRIP || shouldSkipDrawing())
        return S_OK;

    setDirty(primitiveTopology, (D3D_PRIMITIVE_TOPOLOGY)PrimitiveType, DirtyPrimitiveTopology);

    LOCK_GUARD();
    flush();

    if (enableInstancing)
        deviceContext->DrawInstanced(calculateIndexCount(PrimitiveType, PrimitiveCount), instanceCount, StartVertex, 0);
    else
        deviceContext->Draw(calculateIndexCount(PrimitiveType, PrimitiveCount), StartVertex);

    return S_OK;
}       
        
HRESULT Device::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
    if (PrimitiveType > D3DPT_TRIANGLESTRIP || shouldSkipDrawing())
        return S_OK;

    setDirty(primitiveTopology, (D3D_PRIMITIVE_TOPOLOGY)PrimitiveType, DirtyPrimitiveTopology);

    LOCK_GUARD();
    flush();

    if (enableInstancing)
        deviceContext->DrawIndexedInstanced(calculateIndexCount(PrimitiveType, primCount), instanceCount, startIndex, BaseVertexIndex, 0);
    else
        deviceContext->DrawIndexed(calculateIndexCount(PrimitiveType, primCount), startIndex, BaseVertexIndex);

    return S_OK;
}       
        
HRESULT Device::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    if (PrimitiveType > D3DPT_TRIANGLESTRIP || shouldSkipDrawing())
        return S_OK;

    setDirty(vertexStrides[0], VertexStreamZeroStride, DirtyVertexBuffer);
    setDirty(vertexOffsets[0], 0u, DirtyVertexBuffer);
    setDirty(primitiveTopology, (D3D_PRIMITIVE_TOPOLOGY)PrimitiveType, DirtyPrimitiveTopology);

    // Calculate total byte size
    const UINT vertexCount = calculateIndexCount(PrimitiveType, PrimitiveCount); // index count == vertex count
    const UINT vertexSize = vertexCount * VertexStreamZeroStride;

    LOCK_GUARD();

    // Reserve vertex buffer
    if (!reserveUploadVertexBuffer(pVertexStreamZeroData, vertexSize))
    {
        // We need to copy if buffer wasn't reallocated
        D3D11_MAPPED_SUBRESOURCE subResource;
        deviceContext->Map(uploadVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
        memcpy(subResource.pData, pVertexStreamZeroData, vertexSize);
        deviceContext->Unmap(uploadVertexBuffer.Get(), 0);
    }

    // Set first vertex buffer
    setDirty(vertexBuffers[0], uploadVertexBuffer.Get(), DirtyVertexBuffer);

    // Draw
    flush();
    deviceContext->Draw(vertexCount, 0);

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
    setDirty(vertexDeclaration, pDecl, DirtyVertexShader);

    // Totally not for the PBR shaders mod, nuh-huh.
    UINT flags = globalsShared.flags & ~(SHARED_FLAGS_HAS_10_BIT_NORMAL | SHARED_FLAGS_HAS_BINORMAL);
    if (pDecl)
    {
        if (pDecl->getHas10BitNormal()) flags |= SHARED_FLAGS_HAS_10_BIT_NORMAL;
        if (pDecl->getHasBinormal()) flags |= SHARED_FLAGS_HAS_BINORMAL;
    }

    setDirty(globalsShared.flags, flags, DirtySharedConstant);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexDeclaration, VertexDeclaration** ppDecl)

HRESULT Device::SetFVF(DWORD FVF)
{
    auto& fvf = fvfMap[FVF];
    if (!fvf)
        fvf.Attach(new VertexDeclaration(FVF));

    setDirty(vertexDeclaration, fvf.Get(), DirtyVertexShader);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetFVF, DWORD* pFVF)

HRESULT Device::CreateVertexShader(CONST DWORD* pFunction, VertexShader** ppShader, DWORD FunctionSize)
{
    ShaderCache::getVertexShader(device.Get(), const_cast<DWORD*>(pFunction), FunctionSize, ppShader);
    return S_OK;
}

HRESULT Device::SetVertexShader(VertexShader* pShader)
{
    setDirty(vertexShader, pShader, DirtyVertexShader);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShader, VertexShader** ppShader)

HRESULT Device::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    memcpy(&globalsVS.c[StartRegister], pConstantData, Vector4fCount * sizeof(FLOAT[4]));
    dirty |= 1 << DirtyVertexConstant;

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantF, UINT StartRegister, float* pConstantData, UINT Vector4fCount)

HRESULT Device::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantI, UINT StartRegister, int* pConstantData, UINT Vector4iCount)

HRESULT Device::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
    const size_t mask = 1 << StartRegister;
    const size_t value = pConstantData[0] << StartRegister;

    setDirty(globalsShared.booleans, (globalsShared.booleans & ~mask) | value, DirtySharedConstant);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantB, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)

HRESULT Device::SetStreamSource(UINT StreamNumber, VertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride)
{
    setDirty(vertexBuffers[StreamNumber], pStreamData ? reinterpret_cast<ID3D11Buffer*>(pStreamData->getResource()) : nullptr, DirtyVertexBuffer);
    setDirty(vertexStrides[StreamNumber], Stride, DirtyVertexBuffer);
    setDirty(vertexOffsets[StreamNumber], OffsetInBytes, DirtyVertexBuffer);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetStreamSource, UINT StreamNumber, VertexBuffer** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)

HRESULT Device::SetStreamSourceFreq(UINT StreamNumber, UINT Setting)
{
    if (StreamNumber != 0)
        return S_OK;

    setDirty(enableInstancing, (Setting & D3DSTREAMSOURCE_INDEXEDDATA) != 0, DirtyVertexShader);
    instanceCount = Setting & 0x3FFFFFFF;

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetStreamSourceFreq, UINT StreamNumber, UINT* pSetting)

HRESULT Device::SetIndices(IndexBuffer* pIndexData)
{
    setDirty(indexBuffer, pIndexData, DirtyIndexBuffer);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetIndices, IndexBuffer** ppIndexData)

HRESULT Device::CreatePixelShader(CONST DWORD* pFunction, PixelShader** ppShader, DWORD FunctionSize)
{
    ShaderCache::getPixelShader(device.Get(), const_cast<DWORD*>(pFunction), FunctionSize, ppShader);
    return S_OK;
}

HRESULT Device::SetPixelShader(PixelShader* pShader)
{
    setDirty(pixelShader, pShader, DirtyPixelShader);
    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShader, PixelShader** ppShader)

HRESULT Device::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    memcpy(&globalsPS.c[StartRegister], pConstantData, Vector4fCount * sizeof(FLOAT[4]));
    dirty |= 1 << DirtyPixelConstant;

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
    const size_t mask = 1 << (16 + StartRegister);
    const size_t value = pConstantData[0] << (16 + StartRegister);

    setDirty(globalsShared.booleans, (globalsShared.booleans & ~mask) | value, DirtySharedConstant);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantB, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)

FUNCTION_STUB(HRESULT, Device::DrawRectPatch, UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)

FUNCTION_STUB(HRESULT, Device::DrawTriPatch, UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)

FUNCTION_STUB(HRESULT, Device::DeletePatch, UINT Handle)

FUNCTION_STUB(HRESULT, Device::CreateQuery, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)