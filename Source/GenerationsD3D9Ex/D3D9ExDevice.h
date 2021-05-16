#pragma once

#include "D3D9ExUnknown.h"

class D3D9Ex;
class D3D9ExBaseTexture;
class D3D9ExCubeTexture;
class D3D9ExIndexBuffer;
class D3D9ExPixelShader;
class D3D9ExSurface;
class D3D9ExTexture;
class D3D9ExVertexBuffer;
class D3D9ExVertexDeclaration;
class D3D9ExVertexShader;
class D3D9on11RenderTargetSurface;

class D3D9ExDevice : public D3D9ExUnknown
{
    struct StreamSource
    {
        D3D9ExVertexBuffer* dxpVertexBuffer{};
        UINT OffsetInBytes{};
        UINT Stride{};
    };

    IDirect3DDevice9Ex* d3dDevice;
    D3D9Ex* dxp;

    D3D9ExSurface* dxpBackBuffer;
    std::array<D3D9ExSurface*, 4> dxpRenderTargets;
    D3D9ExSurface* dxpDepthStencil;
    std::array<D3D9ExBaseTexture*, 16> dxpStageTextures;
    D3D9ExVertexDeclaration* dxpVertexDeclaration;
    D3D9ExVertexShader* dxpVertexShader;
    std::array<StreamSource, 8> streamSources;
    D3D9ExIndexBuffer* dxpIndexBuffer;
    D3D9ExPixelShader* dxpPixelShader;
    std::array<BOOL, 16> boolParamsPs;
    std::array<BOOL, 16> boolParamsVs;

public:
    D3D9ExDevice(D3D9Ex* d3d, IDirect3DDevice9Ex* d3dDevice);
    ~D3D9ExDevice();

    virtual HRESULT TestCooperativeLevel() final;
    virtual UINT GetAvailableTextureMem() final;
    virtual HRESULT EvictManagedResources() final;
    virtual HRESULT GetDirect3D(D3D9Ex** ppD3D9) final;
    virtual HRESULT GetDeviceCaps(D3DCAPS9* pCaps) final;
    virtual HRESULT GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) final;
    virtual HRESULT GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters) final;
    virtual HRESULT SetCursorProperties(UINT XHotSpot, UINT YHotSpot, D3D9ExSurface* pCursorBitmap) final;
    virtual void SetCursorPosition(int X, int Y, DWORD Flags) final;
    virtual BOOL ShowCursor(BOOL bShow) final;
    virtual HRESULT CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain) final;
    virtual HRESULT GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) final;
    virtual UINT GetNumberOfSwapChains() final;
    virtual HRESULT Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) final;
    virtual HRESULT Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) final;
    virtual HRESULT GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, D3D9ExSurface** ppBackBuffer) final;
    virtual HRESULT GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) final;
    virtual HRESULT SetDialogBoxMode(BOOL bEnableDialogs) final;
    virtual void SetGammaRamp(UINT iSwapChain, DWORD Flags, const D3DGAMMARAMP* pRamp) final;
    virtual void GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) final;
    virtual HRESULT CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, D3D9ExTexture** ppTexture, HANDLE* pSharedHandle) final;
    virtual HRESULT CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle) final;
    virtual HRESULT CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, D3D9ExCubeTexture** ppCubeTexture, HANDLE* pSharedHandle) final;
    virtual HRESULT CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, D3D9ExVertexBuffer** ppVertexBuffer, HANDLE* pSharedHandle) final;
    virtual HRESULT CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, D3D9ExIndexBuffer** ppIndexBuffer, HANDLE* pSharedHandle) final;
    virtual HRESULT CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, D3D9ExSurface** ppSurface, HANDLE* pSharedHandle) final;
    virtual HRESULT CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, D3D9ExSurface** ppSurface, HANDLE* pSharedHandle) final;
    virtual HRESULT UpdateSurface(D3D9ExSurface* pSourceSurface, const RECT* pSourceRect, D3D9ExSurface* pDestinationSurface, const POINT* pDestPoint) final;
    virtual HRESULT UpdateTexture(D3D9ExBaseTexture* pSourceTexture, D3D9ExBaseTexture* pDestinationTexture) final;
    virtual HRESULT GetRenderTargetData(D3D9ExSurface* pRenderTarget, D3D9ExSurface* pDestSurface) final;
    virtual HRESULT GetFrontBufferData(UINT iSwapChain, D3D9ExSurface* pDestSurface) final;
    virtual HRESULT StretchRect(D3D9ExSurface* pSourceSurface, const RECT* pSourceRect, D3D9ExSurface* pDestSurface, const RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) final;
    virtual HRESULT ColorFill(D3D9ExSurface* pSurface, const RECT* pRect, D3DCOLOR color) final;
    virtual HRESULT CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, D3D9ExSurface** ppSurface, HANDLE* pSharedHandle) final;
    virtual HRESULT SetRenderTarget(DWORD RenderTargetIndex, D3D9ExSurface* pRenderTarget) final;
    virtual HRESULT GetRenderTarget(DWORD RenderTargetIndex, D3D9ExSurface** ppRenderTarget) final;
    virtual HRESULT SetDepthStencilSurface(D3D9ExSurface* pNewZStencil) final;
    virtual HRESULT GetDepthStencilSurface(D3D9ExSurface** ppZStencilSurface) final;
    virtual HRESULT BeginScene() final;
    virtual HRESULT EndScene() final;
    virtual HRESULT Clear(DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) final;
    virtual HRESULT SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix) final;
    virtual HRESULT GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) final;
    virtual HRESULT MultiplyTransform(D3DTRANSFORMSTATETYPE, const D3DMATRIX*) final;
    virtual HRESULT SetViewport(const D3DVIEWPORT9* pViewport) final;
    virtual HRESULT GetViewport(D3DVIEWPORT9* pViewport) final;
    virtual HRESULT SetMaterial(const D3DMATERIAL9* pMaterial) final;
    virtual HRESULT GetMaterial(D3DMATERIAL9* pMaterial) final;
    virtual HRESULT SetLight(DWORD Index, const D3DLIGHT9*) final;
    virtual HRESULT GetLight(DWORD Index, D3DLIGHT9*) final;
    virtual HRESULT LightEnable(DWORD Index, BOOL Enable) final;
    virtual HRESULT GetLightEnable(DWORD Index, BOOL* pEnable) final;
    virtual HRESULT SetClipPlane(DWORD Index, const float* pPlane) final;
    virtual HRESULT GetClipPlane(DWORD Index, float* pPlane) final;
    virtual HRESULT SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) final;
    virtual HRESULT GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) final;
    virtual HRESULT CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB) final;
    virtual HRESULT BeginStateBlock() final;
    virtual HRESULT EndStateBlock(IDirect3DStateBlock9** ppSB) final;
    virtual HRESULT SetClipStatus(const D3DCLIPSTATUS9* pClipStatus) final;
    virtual HRESULT GetClipStatus(D3DCLIPSTATUS9* pClipStatus) final;
    virtual HRESULT GetTexture(DWORD Stage, D3D9ExBaseTexture** ppTexture) final;
    virtual HRESULT SetTexture(DWORD Stage, D3D9ExBaseTexture* pTexture) final;
    virtual HRESULT GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) final;
    virtual HRESULT SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) final;
    virtual HRESULT GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) final;
    virtual HRESULT SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) final;
    virtual HRESULT ValidateDevice(DWORD* pNumPasses) final;
    virtual HRESULT SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries) final;
    virtual HRESULT GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) final;
    virtual HRESULT SetCurrentTexturePalette(UINT PaletteNumber) final;
    virtual HRESULT GetCurrentTexturePalette(UINT* PaletteNumber) final;
    virtual HRESULT SetScissorRect(const RECT* pRect) final;
    virtual HRESULT GetScissorRect(RECT* pRect) final;
    virtual HRESULT SetSoftwareVertexProcessing(BOOL bSoftware) final;
    virtual BOOL GetSoftwareVertexProcessing() final;
    virtual HRESULT SetNPatchMode(float nSegments) final;
    virtual float GetNPatchMode() final;
    virtual HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) final;
    virtual HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) final;
    virtual HRESULT DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) final;
    virtual HRESULT DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) final;
    virtual HRESULT ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, D3D9ExVertexBuffer* pDestBuffer, D3D9ExVertexDeclaration* pVertexDecl, DWORD Flags) final;
    virtual HRESULT CreateVertexDeclaration(const D3DVERTEXELEMENT9* pVertexElements, D3D9ExVertexDeclaration** ppDecl) final;
    virtual HRESULT SetVertexDeclaration(D3D9ExVertexDeclaration* pDecl) final;
    virtual HRESULT GetVertexDeclaration(D3D9ExVertexDeclaration** ppDecl) final;
    virtual HRESULT SetFVF(DWORD FVF) final;
    virtual HRESULT GetFVF(DWORD* pFVF) final;
    virtual HRESULT CreateVertexShader(const DWORD* pFunction, D3D9ExVertexShader** ppShader, DWORD FunctionSize) final;
    virtual HRESULT SetVertexShader(D3D9ExVertexShader* pShader) final;
    virtual HRESULT GetVertexShader(D3D9ExVertexShader** ppShader) final;
    virtual HRESULT SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount) final;
    virtual HRESULT GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) final;
    virtual HRESULT SetVertexShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount) final;
    virtual HRESULT GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) final;
    virtual HRESULT SetVertexShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount) final;
    virtual HRESULT GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) final;
    virtual HRESULT SetStreamSource(UINT StreamNumber, D3D9ExVertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride) final;
    virtual HRESULT GetStreamSource(UINT StreamNumber, D3D9ExVertexBuffer** ppStreamData, UINT* pOffsetInBytes, UINT* pStride) final;
    virtual HRESULT SetStreamSourceFreq(UINT StreamNumber, UINT Setting) final;
    virtual HRESULT GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting) final;
    virtual HRESULT SetIndices(D3D9ExIndexBuffer* pIndexData) final;
    virtual HRESULT GetIndices(D3D9ExIndexBuffer** ppIndexData) final;
    virtual HRESULT CreatePixelShader(const DWORD* pFunction, D3D9ExPixelShader** ppShader, DWORD FunctionSize) final;
    virtual HRESULT SetPixelShader(D3D9ExPixelShader* pShader) final;
    virtual HRESULT GetPixelShader(D3D9ExPixelShader** ppShader) final;
    virtual HRESULT SetPixelShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount) final;
    virtual HRESULT GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) final;
    virtual HRESULT SetPixelShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount) final;
    virtual HRESULT GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) final;
    virtual HRESULT SetPixelShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount) final;
    virtual HRESULT GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) final;
    virtual HRESULT DrawRectPatch(UINT Handle, const float* pNumSegs, const D3DRECTPATCH_INFO* pRectPatchInfo) final;
    virtual HRESULT DrawTriPatch(UINT Handle, const float* pNumSegs, const D3DTRIPATCH_INFO* pTriPatchInfo) final;
    virtual HRESULT DeletePatch(UINT Handle) final;
    virtual HRESULT CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) final;

    IDirect3DDevice9Ex* GetD3DDevice()
    {
        return d3dDevice;
    }
};