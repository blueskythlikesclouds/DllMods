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
    IDirect3DDevice9Ex* d3dDevice;
    D3D9Ex* dxp;

    D3D9ExSurface* dxpBackBuffer;
    std::array<D3D9ExSurface*, 4> dxpRenderTargets;
    D3D9ExSurface* dxpDepthStencil;
    std::array<D3D9ExBaseTexture*, 16> dxpStageTextures;
    D3D9ExVertexDeclaration* dxpVertexDeclaration;
    D3D9ExVertexShader* dxpVertexShader;
    std::array<D3D9ExVertexBuffer*, 8> dxpVertexBuffers;
    D3D9ExIndexBuffer* dxpIndexBuffer;
    D3D9ExPixelShader* dxpPixelShader;

public:
    D3D9ExDevice(D3D9Ex* d3d, IDirect3DDevice9Ex* d3dDevice);
    ~D3D9ExDevice();

    virtual HRESULT TestCooperativeLevel();
    virtual UINT GetAvailableTextureMem();
    virtual HRESULT EvictManagedResources();
    virtual HRESULT GetDirect3D(D3D9Ex** ppD3D9);
    virtual HRESULT GetDeviceCaps(D3DCAPS9* pCaps);
    virtual HRESULT GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode);
    virtual HRESULT GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters);
    virtual HRESULT SetCursorProperties(UINT XHotSpot, UINT YHotSpot, D3D9ExSurface* pCursorBitmap);
    virtual void SetCursorPosition(int X, int Y, DWORD Flags);
    virtual BOOL ShowCursor(BOOL bShow);
    virtual HRESULT CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain);
    virtual HRESULT GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain);
    virtual UINT GetNumberOfSwapChains();
    virtual HRESULT Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);
    virtual HRESULT Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
    virtual HRESULT GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, D3D9ExSurface** ppBackBuffer);
    virtual HRESULT GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus);
    virtual HRESULT SetDialogBoxMode(BOOL bEnableDialogs);
    virtual void SetGammaRamp(UINT iSwapChain, DWORD Flags, const D3DGAMMARAMP* pRamp);
    virtual void GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp);
    virtual HRESULT CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, D3D9ExTexture** ppTexture, HANDLE* pSharedHandle);
    virtual HRESULT CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle);
    virtual HRESULT CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, D3D9ExCubeTexture** ppCubeTexture, HANDLE* pSharedHandle);
    virtual HRESULT CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, D3D9ExVertexBuffer** ppVertexBuffer, HANDLE* pSharedHandle);
    virtual HRESULT CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, D3D9ExIndexBuffer** ppIndexBuffer, HANDLE* pSharedHandle);
    virtual HRESULT CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, D3D9ExSurface** ppSurface, HANDLE* pSharedHandle);
    virtual HRESULT CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, D3D9ExSurface** ppSurface, HANDLE* pSharedHandle);
    virtual HRESULT UpdateSurface(D3D9ExSurface* pSourceSurface, const RECT* pSourceRect, D3D9ExSurface* pDestinationSurface, const POINT* pDestPoint);
    virtual HRESULT UpdateTexture(D3D9ExBaseTexture* pSourceTexture, D3D9ExBaseTexture* pDestinationTexture);
    virtual HRESULT GetRenderTargetData(D3D9ExSurface* pRenderTarget, D3D9ExSurface* pDestSurface);
    virtual HRESULT GetFrontBufferData(UINT iSwapChain, D3D9ExSurface* pDestSurface);
    virtual HRESULT StretchRect(D3D9ExSurface* pSourceSurface, const RECT* pSourceRect, D3D9ExSurface* pDestSurface, const RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter);
    virtual HRESULT ColorFill(D3D9ExSurface* pSurface, const RECT* pRect, D3DCOLOR color);
    virtual HRESULT CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, D3D9ExSurface** ppSurface, HANDLE* pSharedHandle);
    virtual HRESULT SetRenderTarget(DWORD RenderTargetIndex, D3D9ExSurface* pRenderTarget);
    virtual HRESULT GetRenderTarget(DWORD RenderTargetIndex, D3D9ExSurface** ppRenderTarget);
    virtual HRESULT SetDepthStencilSurface(D3D9ExSurface* pNewZStencil);
    virtual HRESULT GetDepthStencilSurface(D3D9ExSurface** ppZStencilSurface);
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
    virtual HRESULT GetTexture(DWORD Stage, D3D9ExBaseTexture** ppTexture);
    virtual HRESULT SetTexture(DWORD Stage, D3D9ExBaseTexture* pTexture);
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
    virtual HRESULT ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, D3D9ExVertexBuffer* pDestBuffer, D3D9ExVertexDeclaration* pVertexDecl, DWORD Flags);
    virtual HRESULT CreateVertexDeclaration(const D3DVERTEXELEMENT9* pVertexElements, D3D9ExVertexDeclaration** ppDecl);
    virtual HRESULT SetVertexDeclaration(D3D9ExVertexDeclaration* pDecl);
    virtual HRESULT GetVertexDeclaration(D3D9ExVertexDeclaration** ppDecl);
    virtual HRESULT SetFVF(DWORD FVF);
    virtual HRESULT GetFVF(DWORD* pFVF);
    virtual HRESULT CreateVertexShader(const DWORD* pFunction, D3D9ExVertexShader** ppShader, DWORD FunctionSize);
    virtual HRESULT SetVertexShader(D3D9ExVertexShader* pShader);
    virtual HRESULT GetVertexShader(D3D9ExVertexShader** ppShader);
    virtual HRESULT SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount);
    virtual HRESULT GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount);
    virtual HRESULT SetVertexShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount);
    virtual HRESULT GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount);
    virtual HRESULT SetVertexShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount);
    virtual HRESULT GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount);
    virtual HRESULT SetStreamSource(UINT StreamNumber, D3D9ExVertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride);
    virtual HRESULT GetStreamSource(UINT StreamNumber, D3D9ExVertexBuffer** ppStreamData, UINT* pOffsetInBytes, UINT* pStride);
    virtual HRESULT SetStreamSourceFreq(UINT StreamNumber, UINT Setting);
    virtual HRESULT GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting);
    virtual HRESULT SetIndices(D3D9ExIndexBuffer* pIndexData);
    virtual HRESULT GetIndices(D3D9ExIndexBuffer** ppIndexData);
    virtual HRESULT CreatePixelShader(const DWORD* pFunction, D3D9ExPixelShader** ppShader, DWORD FunctionSize);
    virtual HRESULT SetPixelShader(D3D9ExPixelShader* pShader);
    virtual HRESULT GetPixelShader(D3D9ExPixelShader** ppShader);
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

    IDirect3DDevice9Ex* GetD3DDevice()
    {
        return d3dDevice;
    }
};