#pragma once

static FUNCTION_PTR(void*, __thiscall, createYggTexture, 0x7884C0, 
    YggSchedulerSubInternal* This, const boost::shared_ptr<YggTexture>& yggTexture, float widthScale, float heightScale, uint32_t levels, uint32_t usage, D3DFORMAT format, D3DPOOL pool, uint32_t unknown);

static FUNCTION_PTR(void*, __thiscall, getDefaultFramebuffer, 0x784A60, 
    YggJob* This, const boost::shared_ptr<YggTexture>& yggTexture);

static FUNCTION_PTR(void*, __thiscall, getSymbol, 0x6644C0, 
    SymbolPtr* This, const char* name);

static FUNCTION_PTR(void*, __thiscall, getTextureSymbol, 0x7848D0, 
    YggJob* This, const boost::shared_ptr<YggTexture>& texture, const SymbolPtr& symbol);

static FUNCTION_PTR(void*, __thiscall, getSurfaceSymbol, 0x7847B0,
    YggJob* This, const boost::shared_ptr<YggSurface>& surface, const SymbolPtr& symbol);

static FUNCTION_PTR(void*, __thiscall, getYggSurface, 0x78AE30, 
    YggTexture* This, const boost::shared_ptr<YggSurface>& surface, uint32_t levelIndex, uint32_t mipIndex);

static FUNCTION_PTR(void*, __thiscall, loadShaderDataPair, 0x789DB0, 
    YggScheduler* This, ShaderDataPair& pair, const char* vertexShaderName, const char* pixelShaderName);

static FUNCTION_PTR(void*, __thiscall, loadYggPicture, 0x789DD0, 
    YggScheduler* This, const boost::shared_ptr<YggPicture>& picture, const char* name);

static FUNCTION_PTR(void*, __thiscall, renderQuad, 0x786620, 
    YggSchedulerSubInternal* This, uint32_t unknown, float x, float y);

static FUNCTION_PTR(void*, __thiscall, restoreTexture, 0x412120, 
    YggSchedulerSubInternal* This, uint32_t index);

static FUNCTION_PTR(void*, __thiscall, setDefaultFramebuffer, 0x784D00, 
    YggJob* This, const boost::shared_ptr<YggTexture>& yggTexture);

static FUNCTION_PTR(void*, __thiscall, setDepthStencil, 0x785D10, 
    YggSchedulerSubInternal* This, const boost::shared_ptr<YggSurface>& surface);

static FUNCTION_PTR(void*, __thiscall, setPixelShaderConstantF, 0x411BF0, 
    RenderingInfrastructureInternal* This, const SymbolPtr& symbol, ShaderConstantData* data, uint32_t count, uint32_t offset, const boost::shared_ptr<PixelShaderData>& pixelShaderData);

static FUNCTION_PTR(void*, __thiscall, setRenderState, 0x40BB20, 
    RenderingInfrastructureInternal* This, D3DRENDERSTATETYPE type, uint32_t value);

static FUNCTION_PTR(void*, __thiscall, setRenderTarget, 0x785D00,
    YggSchedulerSubInternal* This, uint32_t index, const boost::shared_ptr<YggSurface>& surface);

static FUNCTION_PTR(void*, __thiscall, setSamplerAddressMode, 0x411CA0, 
    YggSchedulerSubInternal* This, uint32_t index, uint32_t value);

static FUNCTION_PTR(void*, __thiscall, setSamplerFilter, 0x411CF0, 
    YggSchedulerSubInternal* This, uint32_t index, D3DTEXTUREFILTERTYPE migFilter, D3DTEXTUREFILTERTYPE magFilter, D3DTEXTUREFILTERTYPE mipFilter);

static FUNCTION_PTR(void*, __thiscall, setShader, 0x415EE0, 
    YggSchedulerSubInternal* This, const boost::shared_ptr<VertexShaderData>& vertexShaderData, const boost::shared_ptr<PixelShaderData>& pixelShaderData);

static FUNCTION_PTR(void*, __thiscall, setTexture, 0x7865A0, 
    YggSchedulerSubInternal* This, uint32_t index, const boost::shared_ptr<YggTexture>& yggTexture);

static FUNCTION_PTR(void*, __thiscall, setTextureSymbol, 0x784BF0, 
    YggJob* This, const SymbolPtr& symbol, const boost::shared_ptr<YggTexture>& texture);

static FUNCTION_PTR(void*, __thiscall, setSurfaceSymbol, 0x784BF0,
    YggJob* This, const SymbolPtr& symbol, const boost::shared_ptr<YggSurface>& surface);

static FUNCTION_PTR(void, __thiscall, destructString, 0x661550, 
    SharedString* This);

static FUNCTION_PTR(void*, __thiscall, constructString, 0x6621A0, 
    SharedString* This, const char* value);