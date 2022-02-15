#include "Device.h"

#include "Buffer.h"
#include "DepthStencilSurface.h"
#include "DepthStencilTexture.h"
#include "RenderTargetSurface.h"
#include "RenderTargetTexture.h"
#include "Surface.h"
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

void Device::updatePipelineState()
{
    if (dirty[DSI_RenderTarget])
    {
        ID3D11RenderTargetView* renderTargetViews[_countof(renderTargets)];

        size_t i;
        for (i = 0; i < _countof(renderTargets) && renderTargets[i]; i++)
            renderTargetViews[i] = renderTargets[i]->getRTV();

        deviceContext->OMSetRenderTargets(i, renderTargetViews, depthStencil ? depthStencil->getDSV() : nullptr);
    }

    if (dirty[DSI_Viewport])
        deviceContext->RSSetViewports(1, &viewport);

    if (dirty[DSI_DepthStencilState])
    {
        const size_t hash = generateCrc32Hash(0, &depthStencilState, sizeof(depthStencilState));
        const auto pair = depthStencilStates.find(hash);

        ID3D11DepthStencilState* depthStencilStateObj = nullptr;

        if (pair != depthStencilStates.end())
            depthStencilStateObj = pair->second.Get();
        else
        {
            device->CreateDepthStencilState(&depthStencilState, &depthStencilStateObj);
            depthStencilStates.insert(std::make_pair(hash, depthStencilStateObj));
            depthStencilStateObj->Release();
        }

        deviceContext->OMSetDepthStencilState(depthStencilStateObj, 0);
    }

    if (dirty[DSI_RasterizerState])
    {
        const size_t hash = generateCrc32Hash(0, &rasterizerState, sizeof(rasterizerState));
        const auto pair = rasterizerStates.find(hash);

        ID3D11RasterizerState* rasterizerStateObj = nullptr;

        if (pair != rasterizerStates.end())
            rasterizerStateObj = pair->second.Get();
        else
        {
            device->CreateRasterizerState(&rasterizerState, &rasterizerStateObj);
            rasterizerStates.insert(std::make_pair(hash, rasterizerStateObj));
            rasterizerStateObj->Release();
        }

        deviceContext->RSSetState(rasterizerStateObj);
    }

    if (dirty[DSI_AlphaTest])
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubResource;
        deviceContext->Map(alphaTestBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
        memcpy(mappedSubResource.pData, &alphaTestEnable, 8);
        deviceContext->Unmap(alphaTestBuffer.Get(), 0);

        deviceContext->PSSetConstantBuffers(1, 1, alphaTestBuffer.GetAddressOf());
    }

    if (dirty[DSI_BlendState])
    {
        const size_t hash = generateCrc32Hash(0, &blendState, sizeof(blendState));
        const auto pair = blendStates.find(hash);

        ID3D11BlendState* blendStateObj = nullptr;

        if (pair != blendStates.end())
            blendStateObj = pair->second.Get();
        else
        {
            device->CreateBlendState(&blendState, &blendStateObj);
            blendStates.insert(std::make_pair(hash, blendStateObj));
            blendStateObj->Release();
        }

        const FLOAT blendFactor[4] { 1, 1, 1, 1 };
        deviceContext->OMSetBlendState(blendStateObj, blendFactor, ~0);
    }

    for (size_t i = 0; i < _countof(textures);)
    {
        if (!dirty[DSI_Texture + i])
        {
            ++i;
            continue;
        }

        ID3D11ShaderResourceView* shaderResourceViews[_countof(textures)];

        size_t j;
        for (j = i; j < _countof(textures) && dirty[DSI_Texture + j]; j++)
        {
            setDSI(samplers[j].Filter, (D3D11_FILTER)(samplers[j].Filter & ~0x80), DSI_Sampler + j); // Cleanse sampler state
            shaderResourceViews[j - i] = textures[j] ? textures[j]->getSRV() : nullptr;
        }

        deviceContext->PSSetShaderResources(i, j - i, shaderResourceViews);
        i = j + 1;
    }

    for (size_t i = 0; i < _countof(samplers);)
    {
        if (!dirty[DSI_Sampler + i])
        {
            ++i;
            continue;
        }

        ID3D11SamplerState* samplerStateObjs[_countof(samplers)];

        size_t j;
        for (j = i; j < _countof(samplers) && dirty[DSI_Sampler + j]; j++)
        {
            // g_ShadowMapSampler/g_VerticalShadowMapSampler
            if ((j == 7 || j == 13) && textures[j] && textures[j]->getFormat() == DXGI_FORMAT_D24_UNORM_S8_UINT)
                samplers[j].Filter = (D3D11_FILTER)(samplers[j].Filter | 0x80);

            const size_t hash = generateCrc32Hash(0, &samplers[j], sizeof(samplers[j]));
            const auto pair = samplerStates.find(hash);

            if (pair != samplerStates.end())
                samplerStateObjs[j - i] = pair->second.Get();
            else
            {
                device->CreateSamplerState(&samplers[j], &samplerStateObjs[j - i]);
                samplerStates.insert(std::make_pair(hash, samplerStateObjs[j - i]));
                samplerStateObjs[j - i]->Release();
            }
        }

        deviceContext->PSSetSamplers(i, j - i, samplerStateObjs);
        i = j + 1;
    }

    if (dirty[DSI_ScissorRect])
        deviceContext->RSSetScissorRects(1, &scissorRect);

    if (dirty[DSI_PrimitiveTopology])
        deviceContext->IASetPrimitiveTopology(primitiveTopology);

    if (dirty[DSI_VertexShader] && vertexShader && vertexDeclaration)
    {
        deviceContext->VSSetShader(vertexShader->getVertexShader(), nullptr, 0);
        deviceContext->IASetInputLayout(vertexDeclaration->getInputLayout(device.Get(), vertexShader.Get(), instanceCount > 1));
    }

    if (dirty[DSI_VertexConstant])
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubResource;
        deviceContext->Map(vertexConstantsBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
        memcpy(mappedSubResource.pData, &vertexConstants, sizeof(vertexConstants));
        deviceContext->Unmap(vertexConstantsBuffer.Get(), 0);

        deviceContext->VSSetConstantBuffers(0, 1, vertexConstantsBuffer.GetAddressOf());
    }

    if (dirty[DSI_VertexBuffer])
    {
        ID3D11Buffer* buffers[_countof(vertexBuffers)];

        size_t i;
        for (i = 0; i < _countof(vertexBuffers) && vertexBuffers[i]; i++)
            buffers[i] = vertexBuffers[i].Get();

        deviceContext->IASetVertexBuffers(0, i, buffers, vertexStrides, vertexOffsets);
    }

    if (dirty[DSI_IndexBuffer] && indexBuffer)
        deviceContext->IASetIndexBuffer(reinterpret_cast<ID3D11Buffer*>(indexBuffer->getResource()), indexBuffer->getFormat(), 0);

    if (dirty[DSI_PixelShader] && pixelShader)
        deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    if (dirty[DSI_PixelConstant])
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubResource;
        deviceContext->Map(pixelConstantsBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
        memcpy(mappedSubResource.pData, &pixelConstants, sizeof(pixelConstants));
        deviceContext->Unmap(pixelConstantsBuffer.Get(), 0);

        deviceContext->PSSetConstantBuffers(0, 1, pixelConstantsBuffer.GetAddressOf());
    }

    dirty.reset();
}

void Device::setDSI(void* dest, const void* src, const size_t byteSize, const size_t dirtyStateIndex)
{
    if (memcmp(dest, src, byteSize) == 0)
        return;

    dirty.set(dirtyStateIndex);
    memcpy(dest, src, byteSize);
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

Device::Device(D3DPRESENT_PARAMETERS* presentationParameters)
    : syncInterval(presentationParameters->PresentationInterval == 1 ? 1 : 0)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc{};
    swapChainDesc.BufferDesc.Width = presentationParameters->BackBufferWidth;
    swapChainDesc.BufferDesc.Height = presentationParameters->BackBufferHeight;
    swapChainDesc.BufferDesc.Format = TypeConverter::convert(presentationParameters->BackBufferFormat);
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = presentationParameters->BackBufferCount;
    swapChainDesc.OutputWindow = presentationParameters->hDeviceWindow;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    // Create device
    D3D11CreateDeviceAndSwapChain
    (
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
        &swapChainDesc,
        swapChain.GetAddressOf(),
        device.GetAddressOf(),
        nullptr,
        deviceContext.GetAddressOf()
    );

    SetForegroundWindow(presentationParameters->hDeviceWindow);
    SetFocus(presentationParameters->hDeviceWindow);

    // Init backbuffer
    ComPtr<ID3D11Texture2D> backBuffer;
    swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

    backBufferRenderTarget.Attach(new RenderTargetTexture(this, backBuffer.Get(), DXGI_FORMAT_UNKNOWN));

    depthStencilState = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
    rasterizerState = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
    blendState = CD3D11_BLEND_DESC(CD3D11_DEFAULT());

    for (auto& sampler : samplers)
    {
        sampler = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
        sampler.ComparisonFunc = D3D11_COMPARISON_LESS;
    }

    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = (sizeof(vertexConstants) + 255) & ~0xFF;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    device->CreateBuffer(&bufferDesc, nullptr, vertexConstantsBuffer.GetAddressOf());

    bufferDesc.ByteWidth = (sizeof(pixelConstants) + 255) & ~0xFF;
    device->CreateBuffer(&bufferDesc, nullptr, pixelConstantsBuffer.GetAddressOf());

    bufferDesc.ByteWidth = 16;
    device->CreateBuffer(&bufferDesc, nullptr, alphaTestBuffer.GetAddressOf());

    dirty.set();
}

ID3D11Device* Device::getDevice() const
{
    return device.Get();
}

ID3D11DeviceContext* Device::getDeviceContext() const
{
    return deviceContext.Get();
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
    LOCK_GUARD();

    swapChain->Present(syncInterval, 0);

    return S_OK;
}

HRESULT Device::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, Surface** ppBackBuffer)
{
    backBufferRenderTarget->GetSurfaceLevel(0, ppBackBuffer);

    return S_OK;
}
    
FUNCTION_STUB(HRESULT, Device::GetRasterStatus, UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)

FUNCTION_STUB(HRESULT, Device::SetDialogBoxMode, BOOL bEnableDialogs)

FUNCTION_STUB(void, Device::SetGammaRamp, UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)

FUNCTION_STUB(void, Device::GetGammaRamp, UINT iSwapChain, D3DGAMMARAMP* pRamp)

HRESULT Device::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, Texture** ppTexture, HANDLE* pSharedHandle)
{
    LOCK_GUARD();

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
    desc.Format = Usage != 0 ? TypeConverter::makeTypeless(format) : format;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags =
        Usage & D3DUSAGE_RENDERTARGET ? D3D11_BIND_RENDER_TARGET :
        Usage & D3DUSAGE_DEPTHSTENCIL ? D3D11_BIND_DEPTH_STENCIL : 0;

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
    LOCK_GUARD();

    ComPtr<RenderTargetTexture> renderTargetTex;
    const HRESULT hr = CreateTexture(Width, Height, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, reinterpret_cast<Texture**>(renderTargetTex.GetAddressOf()), pSharedHandle);
    if (FAILED(hr))
        return hr;

    renderTargetTex->GetSurfaceLevel(0, ppSurface);

    return S_OK;
}

HRESULT Device::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, Surface** ppSurface, HANDLE* pSharedHandle)
{
    LOCK_GUARD();

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
    LOCK_GUARD();

    setDSI(renderTargets[RenderTargetIndex], pRenderTarget ? reinterpret_cast<RenderTargetSurface*>(pRenderTarget)->getTexture() : nullptr, DSI_RenderTarget);

    return S_OK;
}

HRESULT Device::GetRenderTarget(DWORD RenderTargetIndex, Surface** ppRenderTarget)
{
    return S_OK;
}

HRESULT Device::SetDepthStencilSurface(Surface* pNewZStencil)
{
    LOCK_GUARD();

    setDSI(depthStencil, pNewZStencil ? reinterpret_cast<DepthStencilSurface*>(pNewZStencil)->getTexture() : nullptr, DSI_RenderTarget);

    return S_OK;
}

HRESULT Device::GetDepthStencilSurface(Surface** ppZStencilSurface)
{
    LOCK_GUARD();

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
            clearFlag |= D3D11_CLEAR_DEPTH;

        deviceContext->ClearDepthStencilView(depthStencil->getDSV(), clearFlag, Z, (UINT8)Stencil);
    }

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::SetTransform, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)

FUNCTION_STUB(HRESULT, Device::GetTransform, D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)

FUNCTION_STUB(HRESULT, Device::MultiplyTransform, D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*)

HRESULT Device::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
    LOCK_GUARD();

    D3D11_VIEWPORT newViewport;
    newViewport.TopLeftX = (float)pViewport->X;
    newViewport.TopLeftY = (float)pViewport->Y;
    newViewport.Width = (float)pViewport->Width;
    newViewport.Height = (float)pViewport->Height;
    newViewport.MinDepth = pViewport->MinZ;
    newViewport.MaxDepth = pViewport->MaxZ;

    setDSI(&viewport, &newViewport, sizeof(D3D11_VIEWPORT), DSI_Viewport);

    return S_OK;
}

HRESULT Device::GetViewport(D3DVIEWPORT9* pViewport)
{
    LOCK_GUARD();

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
    LOCK_GUARD();

    switch (State)
    {
    case D3DRS_ZENABLE:
        setDSI(depthStencilState.DepthEnable, (BOOL)Value, DSI_DepthStencilState);
        break;

    case D3DRS_FILLMODE:
        setDSI(rasterizerState.FillMode, (D3D11_FILL_MODE)Value, DSI_RasterizerState);
        break;

    case D3DRS_ZWRITEENABLE:
        setDSI(depthStencilState.DepthWriteMask, (D3D11_DEPTH_WRITE_MASK)Value, DSI_DepthStencilState);
        break;

    case D3DRS_ALPHATESTENABLE:
        setDSI(alphaTestEnable, (BOOL)Value, DSI_AlphaTest);
        break;

    case D3DRS_SRCBLEND:
        setDSI(blendState.RenderTarget[0].SrcBlend, (D3D11_BLEND)Value, DSI_BlendState);
        break;

    case D3DRS_DESTBLEND:
        setDSI(blendState.RenderTarget[0].DestBlend, (D3D11_BLEND)Value, DSI_BlendState);
        break;

    case D3DRS_CULLMODE:
        setDSI(rasterizerState.CullMode, (D3D11_CULL_MODE)Value, DSI_RasterizerState);
        break;

    case D3DRS_ZFUNC:
        setDSI(depthStencilState.DepthFunc, (D3D11_COMPARISON_FUNC)Value, DSI_DepthStencilState);
        break;

    case D3DRS_ALPHAREF:
        setDSI(alphaRef, (float)Value / 255.0f, DSI_AlphaTest);
        break;

    case D3DRS_ALPHABLENDENABLE:
        setDSI(blendState.RenderTarget[0].BlendEnable, (BOOL)Value, DSI_BlendState);
        break;

    case D3DRS_COLORWRITEENABLE:
        setDSI(blendState.RenderTarget[0].RenderTargetWriteMask, (UINT8)Value, DSI_BlendState);
        break;

    case D3DRS_BLENDOP:
        setDSI(blendState.RenderTarget[0].BlendOp, (D3D11_BLEND_OP)Value, DSI_BlendState);
        break;

    case D3DRS_SLOPESCALEDEPTHBIAS:
        setDSI(rasterizerState.SlopeScaledDepthBias, *(float*)&Value, DSI_RasterizerState);
        break;

    case D3DRS_COLORWRITEENABLE1:
        setDSI(blendState.RenderTarget[1].RenderTargetWriteMask, (UINT8)Value, DSI_BlendState);
        break;

    case D3DRS_COLORWRITEENABLE2:
        setDSI(blendState.RenderTarget[2].RenderTargetWriteMask, (UINT8)Value, DSI_BlendState);
        break;

    case D3DRS_COLORWRITEENABLE3: 
        setDSI(blendState.RenderTarget[3].RenderTargetWriteMask, (UINT8)Value, DSI_BlendState);
        break;

    case D3DRS_DEPTHBIAS:
        setDSI(rasterizerState.DepthBias, (INT)(*(float*)&Value * (1 << 24)), DSI_RasterizerState);
        break;

    case D3DRS_SRCBLENDALPHA:
        setDSI(blendState.RenderTarget[0].SrcBlendAlpha, (D3D11_BLEND)Value, DSI_BlendState);
        break;

    case D3DRS_DESTBLENDALPHA:
        setDSI(blendState.RenderTarget[0].DestBlendAlpha, (D3D11_BLEND)Value, DSI_BlendState);
        break;

    case D3DRS_BLENDOPALPHA:
        setDSI(blendState.RenderTarget[0].BlendOpAlpha, (D3D11_BLEND_OP)Value, DSI_BlendState);
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
    LOCK_GUARD();

    setDSI(textures[Stage], reinterpret_cast<Texture*>(pTexture), DSI_Texture + Stage);

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
    LOCK_GUARD();

    switch (Type)
    {
    case D3DSAMP_ADDRESSU:
        setDSI(samplers[Sampler].AddressU, (D3D11_TEXTURE_ADDRESS_MODE)Value, DSI_Sampler + Sampler);
        break;

    case D3DSAMP_ADDRESSV:
        setDSI(samplers[Sampler].AddressV, (D3D11_TEXTURE_ADDRESS_MODE)Value, DSI_Sampler + Sampler);
        break;

    case D3DSAMP_ADDRESSW:
        setDSI(samplers[Sampler].AddressW, (D3D11_TEXTURE_ADDRESS_MODE)Value, DSI_Sampler + Sampler);
        break;

    case D3DSAMP_BORDERCOLOR:
        FLOAT color[4];
        color[2] = (Value & 0xFF) / 255.0f;
        color[1] = ((Value >> 8) & 0xFF) / 255.0f;
        color[0] = ((Value >> 16) & 0xFF) / 255.0f;
        color[3] = ((Value >> 24) & 0xFF) / 255.0f;

        setDSI(samplers[Sampler].BorderColor, color, sizeof(samplers[Sampler].BorderColor), DSI_Sampler + Sampler);
        break;

    case D3DSAMP_MAGFILTER:
        if (Value == D3DTEXF_LINEAR)
            setDSI(samplers[Sampler].Filter, (D3D11_FILTER)(samplers[Sampler].Filter | 0x4), DSI_Sampler + Sampler);
        else
            setDSI(samplers[Sampler].Filter, (D3D11_FILTER)(samplers[Sampler].Filter & ~0x4), DSI_Sampler + Sampler);

        break;

    case D3DSAMP_MINFILTER:
        if (Value == D3DTEXF_LINEAR)
            setDSI(samplers[Sampler].Filter, (D3D11_FILTER)(samplers[Sampler].Filter | 0x10), DSI_Sampler + Sampler);
        else
            setDSI(samplers[Sampler].Filter, (D3D11_FILTER)(samplers[Sampler].Filter & ~0x10), DSI_Sampler + Sampler);

        break;

    case D3DSAMP_MIPFILTER:
        if (Value == D3DTEXF_LINEAR)
            setDSI(samplers[Sampler].Filter, (D3D11_FILTER)(samplers[Sampler].Filter | 0x1), DSI_Sampler + Sampler);
        else
            setDSI(samplers[Sampler].Filter, (D3D11_FILTER)(samplers[Sampler].Filter & ~0x1), DSI_Sampler + Sampler);

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
    LOCK_GUARD();

    D3D11_RECT newScissorRect;
    newScissorRect.left = pRect->left;
    newScissorRect.top = pRect->top;
    newScissorRect.right = pRect->right;
    newScissorRect.bottom = pRect->bottom;

    setDSI(&scissorRect, &newScissorRect, sizeof(D3D11_RECT), DSI_ScissorRect);

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

    LOCK_GUARD();

    setDSI(primitiveTopology, (D3D_PRIMITIVE_TOPOLOGY)PrimitiveType, DSI_PrimitiveTopology);
    updatePipelineState();

    deviceContext->DrawInstanced(calculateIndexCount(PrimitiveType, PrimitiveCount), instanceCount, StartVertex, 0);

    return S_OK;
}       
        
HRESULT Device::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
    if (PrimitiveType > D3DPT_TRIANGLESTRIP)
        return S_OK;

    LOCK_GUARD();

    setDSI(primitiveTopology, (D3D_PRIMITIVE_TOPOLOGY)PrimitiveType, DSI_PrimitiveTopology);
    updatePipelineState();

    deviceContext->DrawIndexedInstanced(calculateIndexCount(PrimitiveType, primCount), instanceCount, startIndex, BaseVertexIndex, 0);

    return S_OK;
}       
        
HRESULT Device::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    if (PrimitiveType > D3DPT_TRIANGLESTRIP)
        return S_OK;

    LOCK_GUARD();

    const UINT vertexCount = calculateIndexCount(PrimitiveType, PrimitiveCount); // index count == vertex count

    // Calculate total byte size
    const UINT vertexSize = vertexCount * VertexStreamZeroStride;

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
    setDSI(vertexBuffers[0], uploadVertexBuffer.Get(), DSI_VertexBuffer);
    setDSI(vertexStrides[0], VertexStreamZeroStride, DSI_VertexBuffer);
    setDSI(vertexOffsets[0], 0u, DSI_VertexBuffer);

    // Draw
    setDSI(primitiveTopology, (D3D_PRIMITIVE_TOPOLOGY)PrimitiveType, DSI_PrimitiveTopology);
    updatePipelineState();

    deviceContext->DrawInstanced(vertexCount, instanceCount, 0, 0);

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
    LOCK_GUARD();

    setDSI(vertexDeclaration, pDecl, DSI_VertexShader);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexDeclaration, VertexDeclaration** ppDecl)

HRESULT Device::SetFVF(DWORD FVF)
{
    ComPtr<VertexDeclaration> fvf;

    const auto pair = fvfMap.find(FVF);
    if (pair != fvfMap.end())
        fvf = pair->second;

    else
    {
        fvf.Attach(new VertexDeclaration(FVF));
        fvfMap.insert(std::make_pair(FVF, fvf));
    }

    setDSI(vertexDeclaration, fvf.Get(), DSI_VertexShader);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetFVF, DWORD* pFVF)

HRESULT Device::CreateVertexShader(CONST DWORD* pFunction, VertexShader** ppShader, DWORD FunctionSize)
{
    LOCK_GUARD();

    *ppShader = new VertexShader(device.Get(), pFunction, FunctionSize);

    return S_OK;
}

HRESULT Device::SetVertexShader(VertexShader* pShader)
{
    LOCK_GUARD();

    setDSI(vertexShader, pShader, DSI_VertexShader);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShader, VertexShader** ppShader)

HRESULT Device::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    LOCK_GUARD();

    setDSI(&vertexConstants.c[StartRegister], pConstantData, Vector4fCount * sizeof(FLOAT[4]), DSI_VertexConstant);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantF, UINT StartRegister, float* pConstantData, UINT Vector4fCount)

HRESULT Device::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    LOCK_GUARD();

    setDSI(&vertexConstants.i[StartRegister], pConstantData, Vector4iCount * sizeof(INT[4]), DSI_VertexConstant);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantI, UINT StartRegister, int* pConstantData, UINT Vector4iCount)

HRESULT Device::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
    LOCK_GUARD();

    setDSI(&vertexConstants.b[StartRegister], pConstantData, BoolCount * sizeof(BOOL), DSI_VertexConstant);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetVertexShaderConstantB, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)

HRESULT Device::SetStreamSource(UINT StreamNumber, VertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride)
{
    LOCK_GUARD();

    setDSI(vertexBuffers[StreamNumber], pStreamData ? reinterpret_cast<ID3D11Buffer*>(pStreamData->getResource()) : nullptr, DSI_VertexBuffer);
    setDSI(vertexStrides[StreamNumber], Stride, DSI_VertexBuffer);
    setDSI(vertexOffsets[StreamNumber], OffsetInBytes, DSI_VertexBuffer);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetStreamSource, UINT StreamNumber, VertexBuffer** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)

HRESULT Device::SetStreamSourceFreq(UINT StreamNumber, UINT Setting)
{
    if (StreamNumber == 0)
        setDSI(instanceCount, Setting & 0x3FFFFFFF, DSI_VertexBuffer);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetStreamSourceFreq, UINT StreamNumber, UINT* pSetting)

HRESULT Device::SetIndices(IndexBuffer* pIndexData)
{
    LOCK_GUARD();

    setDSI(indexBuffer, pIndexData, DSI_IndexBuffer);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetIndices, IndexBuffer** ppIndexData)

HRESULT Device::CreatePixelShader(CONST DWORD* pFunction, PixelShader** ppShader, DWORD FunctionSize)
{
    LOCK_GUARD();

    device->CreatePixelShader(pFunction, FunctionSize, nullptr, ppShader);

    return S_OK;
}

HRESULT Device::SetPixelShader(PixelShader* pShader)
{
    LOCK_GUARD();

    setDSI(pixelShader, pShader, DSI_PixelShader);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShader, PixelShader** ppShader)

HRESULT Device::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    LOCK_GUARD();

    setDSI(&pixelConstants.c[StartRegister], pConstantData, Vector4fCount * sizeof(FLOAT[4]), DSI_PixelConstant);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantF, UINT StartRegister, float* pConstantData, UINT Vector4fCount)

HRESULT Device::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    LOCK_GUARD();

    setDSI(&pixelConstants.i[StartRegister], pConstantData, Vector4iCount * sizeof(INT[4]), DSI_PixelConstant);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantI, UINT StartRegister, int* pConstantData, UINT Vector4iCount)

HRESULT Device::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
    LOCK_GUARD();

    setDSI(&pixelConstants.b[StartRegister], pConstantData, BoolCount * sizeof(BOOL), DSI_PixelConstant);

    return S_OK;
}

FUNCTION_STUB(HRESULT, Device::GetPixelShaderConstantB, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)

FUNCTION_STUB(HRESULT, Device::DrawRectPatch, UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)

FUNCTION_STUB(HRESULT, Device::DrawTriPatch, UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)

FUNCTION_STUB(HRESULT, Device::DeletePatch, UINT Handle)

FUNCTION_STUB(HRESULT, Device::CreateQuery, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)