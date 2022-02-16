#pragma once

#include "CriticalSection.h"
#include "Unknown.h"

class BaseTexture;
class Buffer;
class CubeTexture;
class D3D9;
class DepthStencilTexture;
class RenderTargetSurface;
class RenderTargetTexture;
class Surface;
class SwapChain;
class Texture;
class VertexDeclaration;
class VertexShader;

typedef Buffer VertexBuffer;
typedef Buffer IndexBuffer;

typedef ID3D11PixelShader PixelShader;

struct alignas(16) VertexConstants
{
    FLOAT c[256][4];
    INT i[16][4];
    BOOL b[16];
};

struct alignas(16) PixelConstants
{
    FLOAT c[224][4];
    INT i[16][4];
    BOOL b[16];
};

class Device : public Unknown
{
    CriticalSection criticalSection;
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> deviceContext;

    std::unique_ptr<SwapChain> swapChain;
    UINT syncInterval;

    ComPtr<RenderTargetTexture> renderTargets[4]{};
    ComPtr<DepthStencilTexture> depthStencil{};
    D3D11_VIEWPORT viewport{};
    D3D11_DEPTH_STENCIL_DESC depthStencilState{};
    D3D11_RASTERIZER_DESC rasterizerState{};
    BOOL alphaTestEnable{};
    FLOAT alphaRef{};
    D3D11_BLEND_DESC blendState{};
    ComPtr<Texture> textures[16];
    D3D11_SAMPLER_DESC samplers[_countof(textures)]{};
    D3D11_RECT scissorRect{};
    D3D_PRIMITIVE_TOPOLOGY primitiveTopology{};
    ComPtr<VertexDeclaration> vertexDeclaration;
    VertexConstants vertexConstants{};
    ComPtr<VertexShader> vertexShader;
    ComPtr<ID3D11Buffer> vertexBuffers[8];
    UINT vertexStrides[_countof(vertexBuffers)]{};
    UINT vertexOffsets[_countof(vertexBuffers)]{};
    UINT instanceCount{1};
    bool enableInstancing{};
    ComPtr<IndexBuffer> indexBuffer;
    ComPtr<PixelShader> pixelShader;
    PixelConstants pixelConstants{};

    std::map<uint32_t, ComPtr<ID3D11DepthStencilState>> depthStencilStates;
    std::map<uint32_t, ComPtr<ID3D11RasterizerState>> rasterizerStates;
    std::map<uint32_t, ComPtr<ID3D11BlendState>> blendStates;
    std::map<uint32_t, ComPtr<ID3D11SamplerState>> samplerStates;
    std::map<uint32_t, ComPtr<VertexDeclaration>> fvfMap;

    ComPtr<ID3D11Buffer> vertexConstantsBuffer;
    ComPtr<ID3D11Buffer> pixelConstantsBuffer;
    ComPtr<ID3D11Buffer> alphaTestBuffer;

    ComPtr<ID3D11Buffer> uploadVertexBuffer;
    size_t uploadVertexBufferSize{};

    enum DirtyStateIndex
    {
        DSI_Present,
        DSI_RenderTarget,
        DSI_Viewport,
        DSI_DepthStencilState,
        DSI_RasterizerState,
        DSI_AlphaTest,
        DSI_BlendState,
        DSI_Texture,
        DSI_Sampler = DSI_Texture + _countof(textures),
        DSI_ScissorRect = DSI_Sampler + _countof(samplers),
        DSI_PrimitiveTopology,
        DSI_VertexShader,
        DSI_VertexConstant,
        DSI_VertexBuffer,
        DSI_IndexBuffer,
        DSI_PixelShader,
        DSI_PixelConstant,
        DSI_Count,
    };

    std::bitset<DSI_Count> dirty;

    void updatePipelineState();
    void setDSI(void* dest, const void* src, size_t byteSize, size_t dirtyStateIndex);

    template<typename T>
    void setDSI(T& dest, const T src, const size_t dirtyStateIndex)
    {
        setDSI(&dest, &src, sizeof(T), dirtyStateIndex);
    }

    template<typename T>
    void setDSI(ComPtr<T>& dest, T* src, const size_t dirtyStateIndex)
    {
        if (dest.Get() == src)
            return;

        dirty.set(dirtyStateIndex);
        dest = src;
    }

    bool reserveUploadVertexBuffer(const void* data, size_t size);

public:
    explicit Device(D3DPRESENT_PARAMETERS* presentationParameters, DXGI_SCALING scaling);

    ID3D11Device* get() const;
    ID3D11DeviceContext* getContext() const;

    std::lock_guard<CriticalSection> lock();

#pragma region D3D9 Device
    virtual HRESULT TestCooperativeLevel();
    virtual UINT GetAvailableTextureMem();
    virtual HRESULT EvictManagedResources();
    virtual HRESULT GetDirect3D(D3D9** ppD3D9);
    virtual HRESULT GetDeviceCaps(D3DCAPS9* pCaps);
    virtual HRESULT GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode);
    virtual HRESULT GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters);
    virtual HRESULT SetCursorProperties(UINT XHotSpot, UINT YHotSpot, Surface* pCursorBitmap);
    virtual void SetCursorPosition(int X, int Y, DWORD Flags);
    virtual BOOL ShowCursor(BOOL bShow);
    virtual HRESULT CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain);
    virtual HRESULT GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain);
    virtual UINT GetNumberOfSwapChains();
    virtual HRESULT Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);
    virtual HRESULT Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
    virtual HRESULT GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, Surface** ppBackBuffer);
    virtual HRESULT GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus);
    virtual HRESULT SetDialogBoxMode(BOOL bEnableDialogs);
    virtual void SetGammaRamp(UINT iSwapChain, DWORD Flags, const D3DGAMMARAMP* pRamp);
    virtual void GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp);
    virtual HRESULT CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, Texture** ppTexture, HANDLE* pSharedHandle);
    virtual HRESULT CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle);
    virtual HRESULT CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, CubeTexture** ppCubeTexture, HANDLE* pSharedHandle);
    virtual HRESULT CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, VertexBuffer** ppVertexBuffer, HANDLE* pSharedHandle);
    virtual HRESULT CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IndexBuffer** ppIndexBuffer, HANDLE* pSharedHandle);
    virtual HRESULT CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, Surface** ppSurface, HANDLE* pSharedHandle);
    virtual HRESULT CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, Surface** ppSurface, HANDLE* pSharedHandle);
    virtual HRESULT UpdateSurface(Surface* pSourceSurface, const RECT* pSourceRect, Surface* pDestinationSurface, const POINT* pDestPoint);
    virtual HRESULT UpdateTexture(BaseTexture* pSourceTexture, BaseTexture* pDestinationTexture);
    virtual HRESULT GetRenderTargetData(Surface* pRenderTarget, Surface* pDestSurface);
    virtual HRESULT GetFrontBufferData(UINT iSwapChain, Surface* pDestSurface);
    virtual HRESULT StretchRect(Surface* pSourceSurface, const RECT* pSourceRect, Surface* pDestSurface, const RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter);
    virtual HRESULT ColorFill(Surface* pSurface, const RECT* pRect, D3DCOLOR color);
    virtual HRESULT CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, Surface** ppSurface, HANDLE* pSharedHandle);
    virtual HRESULT SetRenderTarget(DWORD RenderTargetIndex, Surface* pRenderTarget);
    virtual HRESULT GetRenderTarget(DWORD RenderTargetIndex, Surface** ppRenderTarget);
    virtual HRESULT SetDepthStencilSurface(Surface* pNewZStencil);
    virtual HRESULT GetDepthStencilSurface(Surface** ppZStencilSurface);
    virtual HRESULT BeginScene();
    virtual HRESULT EndScene();
    virtual HRESULT Clear(DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
    virtual HRESULT SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix);
    virtual HRESULT GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix);
    virtual HRESULT MultiplyTransform(D3DTRANSFORMSTATETYPE, const D3DMATRIX*);
    virtual HRESULT SetViewport(const D3DVIEWPORT9* pViewport);
    virtual HRESULT GetViewport(D3DVIEWPORT9* pViewport);
    virtual HRESULT SetMaterial(const D3DMATERIAL9* pMaterial);
    virtual HRESULT GetMaterial(D3DMATERIAL9* pMaterial);
    virtual HRESULT SetLight(DWORD Index, const D3DLIGHT9*);
    virtual HRESULT GetLight(DWORD Index, D3DLIGHT9*);
    virtual HRESULT LightEnable(DWORD Index, BOOL Enable);
    virtual HRESULT GetLightEnable(DWORD Index, BOOL* pEnable);
    virtual HRESULT SetClipPlane(DWORD Index, const float* pPlane);
    virtual HRESULT GetClipPlane(DWORD Index, float* pPlane);
    virtual HRESULT SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
    virtual HRESULT GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue);
    virtual HRESULT CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB);
    virtual HRESULT BeginStateBlock();
    virtual HRESULT EndStateBlock(IDirect3DStateBlock9** ppSB);
    virtual HRESULT SetClipStatus(const D3DCLIPSTATUS9* pClipStatus);
    virtual HRESULT GetClipStatus(D3DCLIPSTATUS9* pClipStatus);
    virtual HRESULT GetTexture(DWORD Stage, BaseTexture** ppTexture);
    virtual HRESULT SetTexture(DWORD Stage, BaseTexture* pTexture);
    virtual HRESULT GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue);
    virtual HRESULT SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
    virtual HRESULT GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue);
    virtual HRESULT SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
    virtual HRESULT ValidateDevice(DWORD* pNumPasses);
    virtual HRESULT SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries);
    virtual HRESULT GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries);
    virtual HRESULT SetCurrentTexturePalette(UINT PaletteNumber);
    virtual HRESULT GetCurrentTexturePalette(UINT* PaletteNumber);
    virtual HRESULT SetScissorRect(const RECT* pRect);
    virtual HRESULT GetScissorRect(RECT* pRect);
    virtual HRESULT SetSoftwareVertexProcessing(BOOL bSoftware);
    virtual BOOL GetSoftwareVertexProcessing();
    virtual HRESULT SetNPatchMode(float nSegments);
    virtual float GetNPatchMode();
    virtual HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
    virtual HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);
    virtual HRESULT DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
    virtual HRESULT DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
    virtual HRESULT ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, VertexBuffer* pDestBuffer, VertexDeclaration* pVertexDecl, DWORD Flags);
    virtual HRESULT CreateVertexDeclaration(const D3DVERTEXELEMENT9* pVertexElements, VertexDeclaration** ppDecl);
    virtual HRESULT SetVertexDeclaration(VertexDeclaration* pDecl);
    virtual HRESULT GetVertexDeclaration(VertexDeclaration** ppDecl);
    virtual HRESULT SetFVF(DWORD FVF);
    virtual HRESULT GetFVF(DWORD* pFVF);
    virtual HRESULT CreateVertexShader(const DWORD* pFunction, VertexShader** ppShader, DWORD FunctionSize);
    virtual HRESULT SetVertexShader(VertexShader* pShader);
    virtual HRESULT GetVertexShader(VertexShader** ppShader);
    virtual HRESULT SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount);
    virtual HRESULT GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount);
    virtual HRESULT SetVertexShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount);
    virtual HRESULT GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount);
    virtual HRESULT SetVertexShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount);
    virtual HRESULT GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount);
    virtual HRESULT SetStreamSource(UINT StreamNumber, VertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride);
    virtual HRESULT GetStreamSource(UINT StreamNumber, VertexBuffer** ppStreamData, UINT* pOffsetInBytes, UINT* pStride);
    virtual HRESULT SetStreamSourceFreq(UINT StreamNumber, UINT Setting);
    virtual HRESULT GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting);
    virtual HRESULT SetIndices(IndexBuffer* pIndexData);
    virtual HRESULT GetIndices(IndexBuffer** ppIndexData);
    virtual HRESULT CreatePixelShader(const DWORD* pFunction, PixelShader** ppShader, DWORD FunctionSize);
    virtual HRESULT SetPixelShader(PixelShader* pShader);
    virtual HRESULT GetPixelShader(PixelShader** ppShader);
    virtual HRESULT SetPixelShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount);
    virtual HRESULT GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount);
    virtual HRESULT SetPixelShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount);
    virtual HRESULT GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount);
    virtual HRESULT SetPixelShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount);
    virtual HRESULT GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount);
    virtual HRESULT DrawRectPatch(UINT Handle, const float* pNumSegs, const D3DRECTPATCH_INFO* pRectPatchInfo);
    virtual HRESULT DrawTriPatch(UINT Handle, const float* pNumSegs, const D3DTRIPATCH_INFO* pTriPatchInfo);
    virtual HRESULT DeletePatch(UINT Handle);
    virtual HRESULT CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery);
#pragma endregion
};