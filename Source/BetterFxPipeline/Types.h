#pragma once

#define _CONCAT2(x, y) x##y
#define CONCAT2(x, y) _CONCAT(x, y)
#define INSERT_PADDING(length) \
    uint8_t CONCAT2(pad, __LINE__)[length]

#define ASSERT_OFFSETOF(type, field, offset) \
    static_assert(offsetof(type, field) == offset, "offsetof assertion failed")

#define ASSERT_SIZEOF(type, size) \
    static_assert(sizeof(type) == size, "sizeof assertion failed")

#pragma region Common

struct Object
{
    void* vtable;
};

ASSERT_SIZEOF(Object, 0x4);

struct SharedString
{
    const char* value; // Length as ushort right before the string starts

    SharedString() : value(nullptr) {}

    SharedString(const char* value);
    ~SharedString();
};

ASSERT_SIZEOF(SharedString, 0x4);

struct Symbol
{
    Symbol* left;
    Symbol* right;
    uint32_t hash;
    const char name[1];
};

typedef Symbol* SymbolPtr;

#pragma endregion

#pragma region Database

enum DatabaseDataFlags : uint8_t
{
    DATABASE_DATA_FLAGS_IS_LOADED = 1
};

struct DatabaseData : Object
{
    uint8_t flags;
    SharedString typeAndName;
};

ASSERT_OFFSETOF(DatabaseData, flags, 0x4);
ASSERT_OFFSETOF(DatabaseData, typeAndName, 0x8);
ASSERT_SIZEOF(DatabaseData, 0xC);

struct PixelShaderData : DatabaseData
{
    INSERT_PADDING(0x18);
};

ASSERT_SIZEOF(PixelShaderData, 0x24);

struct VertexShaderData : DatabaseData
{
    INSERT_PADDING(0x18);
};

ASSERT_SIZEOF(VertexShaderData, 0x24);

typedef std::pair<boost::shared_ptr<VertexShaderData>, boost::shared_ptr<PixelShaderData>> ShaderDataPair;

#pragma endregion

#pragma region Renderer

struct RenderingInfrastructureInternal
{
    struct RenderingInfrastructure* parent;
    DX_PATCH::IDirect3DDevice9* dxpDevice;
    void* globalParametersData;
    uint32_t globalParameterIndex;
    INSERT_PADDING(0x75E0);
};

ASSERT_OFFSETOF(RenderingInfrastructureInternal, parent, 0x0);
ASSERT_OFFSETOF(RenderingInfrastructureInternal, dxpDevice, 0x4);
ASSERT_OFFSETOF(RenderingInfrastructureInternal, globalParametersData, 0x8);
ASSERT_OFFSETOF(RenderingInfrastructureInternal, globalParameterIndex, 0xC);
ASSERT_SIZEOF(RenderingInfrastructureInternal, 0x75F0);

struct RenderingInfrastructure : Object
{
    INSERT_PADDING(0x5C);
    RenderingInfrastructureInternal internal;
};

ASSERT_OFFSETOF(RenderingInfrastructure, internal, 0x60);
ASSERT_SIZEOF(RenderingInfrastructure, 0x7650);

struct ShaderConstantData
{
    float* data;
    uint32_t vector4fCount;
};

ASSERT_OFFSETOF(ShaderConstantData, data, 0x0);
ASSERT_OFFSETOF(ShaderConstantData, vector4fCount, 0x4);
ASSERT_SIZEOF(ShaderConstantData, 0x8);

#pragma endregion

#pragma region Yggdrasill

struct YggSchedulerSubInternal
{
    RenderingInfrastructure* renderingInfrastructure;
    DX_PATCH::IDirect3DDevice9* dxpDevice;
    INSERT_PADDING(0xC0);
    uint32_t width;
    uint32_t height;
};

ASSERT_OFFSETOF(YggSchedulerSubInternal, renderingInfrastructure, 0x0);
ASSERT_OFFSETOF(YggSchedulerSubInternal, dxpDevice, 0x4);
ASSERT_OFFSETOF(YggSchedulerSubInternal, width, 0xC8);
ASSERT_OFFSETOF(YggSchedulerSubInternal, height, 0xCC);

struct YggSchedulerInternal
{
    INSERT_PADDING(0x8);
    RenderingInfrastructure* renderingInfrastructure;
    INSERT_PADDING(0x4);
    YggSchedulerSubInternal* subInternal;
    INSERT_PADDING(0x30);
};

ASSERT_OFFSETOF(YggSchedulerInternal, renderingInfrastructure, 8);
ASSERT_OFFSETOF(YggSchedulerInternal, subInternal, 0x10);
ASSERT_SIZEOF(YggSchedulerInternal, 0x44);

struct YggScheduler : Object
{
    YggSchedulerInternal* internal;
    INSERT_PADDING(0x4);
};

ASSERT_OFFSETOF(YggScheduler, internal, 0x4);
ASSERT_SIZEOF(YggScheduler, 0xC);

struct FxScheduler : YggScheduler
{
    INSERT_PADDING(0x8C);
};

ASSERT_SIZEOF(FxScheduler, 0x98);

struct YggAbstractBuffer : Object
{
    INSERT_PADDING(0x8);
    YggScheduler* scheduler;
    SharedString name;
    INSERT_PADDING(0x8);
};

ASSERT_OFFSETOF(YggAbstractBuffer, scheduler, 0xC);
ASSERT_OFFSETOF(YggAbstractBuffer, name, 0x10);
ASSERT_SIZEOF(YggAbstractBuffer, 0x1C);

struct YggTexture : YggAbstractBuffer
{
    DX_PATCH::IDirect3DBaseTexture9* dxpBaseTexture;
    INSERT_PADDING(0xA0);
};

ASSERT_OFFSETOF(YggTexture, dxpBaseTexture, 0x1C);
ASSERT_SIZEOF(YggTexture, 0xC0);

struct YggPicture : YggTexture
{
    INSERT_PADDING(0x10);
};

ASSERT_SIZEOF(YggPicture, 0xD0);

struct YggSurface : YggAbstractBuffer
{
    DX_PATCH::IDirect3DSurface9* dxpSurface;
    INSERT_PADDING(0x48);
};

ASSERT_OFFSETOF(YggSurface, dxpSurface, 0x1C);
ASSERT_SIZEOF(YggSurface, 0x68);

struct YggJob : Object
{
    INSERT_PADDING(0x8);
    YggScheduler* scheduler;
    INSERT_PADDING(0x38);
};

ASSERT_OFFSETOF(YggJob, scheduler, 0xC);
ASSERT_SIZEOF(YggJob, 0x48);

struct FxShadowMap : YggJob
{
    boost::shared_ptr<YggSurface> renderTarget;
    boost::shared_ptr<YggTexture> shadowMap;
    boost::shared_ptr<YggTexture> shadowMapNoTerrain;
    INSERT_PADDING(0x44);
    uint32_t shadowMapSize;
    INSERT_PADDING(0x4);
};

ASSERT_OFFSETOF(FxShadowMap, renderTarget, 0x48);
ASSERT_OFFSETOF(FxShadowMap, shadowMap, 0x50);
ASSERT_OFFSETOF(FxShadowMap, shadowMapNoTerrain, 0x58);
ASSERT_OFFSETOF(FxShadowMap, shadowMapSize, 0xA4);
ASSERT_SIZEOF(FxShadowMap, 0xAC);

struct FxRenderScene : YggJob
{
    INSERT_PADDING(0x28);
};

ASSERT_SIZEOF(FxRenderScene, 0x70);

struct FxRenderGameScene : FxRenderScene
{
    boost::shared_ptr<YggTexture> frameBuffer;
    boost::shared_ptr<YggTexture> depthStencil;
    INSERT_PADDING(0xB4);
};

ASSERT_OFFSETOF(FxRenderGameScene, frameBuffer, 0x70);
ASSERT_OFFSETOF(FxRenderGameScene, depthStencil, 0x78);
ASSERT_SIZEOF(FxRenderGameScene, 0x134);

#pragma endregion