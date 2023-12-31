#include "Configuration.h"
#include "D3D9.h"
#include "Device.h"
#include "ShaderCache.h"
#include "Texture.h"

#include "../../GenerationsD3D9Ex/MemoryHandler.h"

HOOK(void, __cdecl, LoadPictureData, 0x743DE0,
    hh::mr::CPictureData* pPictureData, const uint8_t* pData, size_t length, hh::mr::CRenderingInfrastructure* pRenderingInfrastructure)
{
    if (pPictureData->m_Flags & hh::db::eDatabaseDataFlags_IsMadeOne)
        return;

    Device* device = (Device*)pRenderingInfrastructure->m_RenderingDevice.m_pD3DDevice;

    ComPtr<ID3D11Resource> texture;
    ComPtr<ID3D11ShaderResourceView> srv;

    HRESULT hr = DirectX::CreateDDSTextureFromMemoryEx(device->get(), nullptr,
        pData, length, 0, D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, 0, 0, false, texture.GetAddressOf(), srv.GetAddressOf());

    if (SUCCEEDED(hr))
    {
#if _DEBUG
        const char* name = pPictureData->m_TypeAndName.c_str() + 15;
        texture->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
#endif

        pPictureData->m_pD3DTexture = (DX_PATCH::IDirect3DBaseTexture9*)(new Texture(device, texture.Get(), srv.Get()));
        pPictureData->m_Type = hh::mr::ePictureType_Texture;
    }
    else
    {
        pPictureData->m_pD3DTexture = nullptr;
        pPictureData->m_Type = hh::mr::ePictureType_Invalid;
    }

    pPictureData->m_Flags |= hh::db::eDatabaseDataFlags_IsMadeOne;
}

typedef VOID(WINAPI* LPD3DXFILL2D)
(
    Eigen::Vector4f* pOut,
    CONST Eigen::Vector2f* pTexCoord,
    CONST Eigen::Vector2f* pTexelSize,
    LPVOID pData
);

HOOK(HRESULT, __stdcall, FillTexture, 0xA55270, Texture* pTexture, LPD3DXFILL2D pFunction, LPVOID pData)
{
    D3DLOCKED_RECT lockedRect;

    if (FAILED(pTexture->LockRect(0, &lockedRect, nullptr, 0)))
        return E_FAIL;

    ComPtr<ID3D11Texture2D> texture;

    if (FAILED(pTexture->getResource()->QueryInterface(IID_PPV_ARGS(texture.GetAddressOf()))))
    {
        pTexture->UnlockRect(0);
        return E_FAIL;
    }

    D3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);

    if (desc.Format != DXGI_FORMAT_B8G8R8A8_UNORM)
    {
        pTexture->UnlockRect(0);
        return E_FAIL;
    }

    const Eigen::Vector2f texelSize(1.0f / (float)desc.Width, 1.0f / (float)desc.Height);

    for (size_t i = 0; i < desc.Width; i++)
    {
        for (size_t j = 0; j < desc.Height; j++)
        {
            Eigen::Vector4f out;
            pFunction(&out, &Eigen::Vector2f((i + 0.5f) / desc.Width, (j + 0.5f) / desc.Height), &texelSize, pData);

            uint8_t* dest = (uint8_t*)lockedRect.pBits + (lockedRect.Pitch * j) + (i * 4);

            dest[0] = (uint8_t)(out.z() * 255.0f);
            dest[1] = (uint8_t)(out.y() * 255.0f);
            dest[2] = (uint8_t)(out.x() * 255.0f);
            dest[3] = (uint8_t)(out.w() * 255.0f);
        }
    }

    pTexture->UnlockRect(0);
    return S_OK;
}

const uint16_t groundSmokeParticleIndexBuffer[] =
{
    0, 1, 2,
    0, 2, 3,
    0, 3, 4,
    0, 4, 5
};

constexpr size_t groundSmokeParticleIndexBufferCount = _countof(groundSmokeParticleIndexBuffer);
constexpr size_t groundSmokeParticleIndexBufferSize = sizeof(groundSmokeParticleIndexBuffer);

// Release mode fails to compile when calling memcpy in inline assembly.
void __stdcall memcpyTrampoline(void* _Dst, void const* _Src, size_t _Size)
{
    memcpy(_Dst, _Src, _Size);
}

void __declspec(naked) groundSmokeParticleCopyIndexBufferMidAsmHook()
{
    static uint32_t returnAddr = 0x11A37CC;

    __asm
    {
        push groundSmokeParticleIndexBufferSize
        push offset groundSmokeParticleIndexBuffer
        push ecx
        call memcpyTrampoline

        jmp[returnAddr]
    }
}

int __stdcall sfdDecodeImpl(int a1, Texture*& texture)
{
    FUNCTION_PTR(int, __cdecl, fun7DFF60, 0x7DFF60, int, int);
    FUNCTION_PTR(int, __cdecl, fun7E08C0, 0x7E08C0, int, int, int);
    FUNCTION_PTR(int, __cdecl, fun7E0BA0, 0x7E0BA0, int, int, void*);

    int v2 = *(int*)(a1 + 296);
    if (!v2)
        return -1;

    fun7DFF60(v2, a1 + 128);

    if (!*(int*)(a1 + 128))
        return -1;

    D3DLOCKED_RECT lockedRect;
    texture->LockRect(0, &lockedRect, nullptr, 0);

    fun7E08C0(*(int*)(a1 + 296), lockedRect.Pitch, *(int*)(a1 + 144));
    fun7E0BA0(*(int*)(a1 + 296), a1 + 128, lockedRect.pBits);

    texture->UnlockRect(0);

    return *(int*)(a1 + 172);
}

void __declspec(naked) sfdDecodeTrampoline()
{
    __asm
    {
        push [esp + 4]
        push esi
        call sfdDecodeImpl
        retn 4
    }
}

HICON __stdcall LoadIconImpl(HINSTANCE hInstance, LPCSTR lpIconName)
{
    return LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(2057));
}

HOOK(D3D9*, __cdecl, Direct3DCreate, 0xA5EDD0, UINT SDKVersion)
{
    ShaderCache::init();
    ShaderCache::load();

    return new D3D9();
}

HOOK(void, WINAPI, MyOutputDebugStringA, &OutputDebugStringA, LPCSTR lpOutputString)
{
    printf(lpOutputString);
    originalMyOutputDebugStringA(lpOutputString);
}

HOOK(void, WINAPI, MyOutputDebugStringW, &OutputDebugStringW, LPCWSTR lpOutputString)
{
    wprintf(lpOutputString);
    originalMyOutputDebugStringW(lpOutputString);
}

HOOK(void, __stdcall, InternalUpdate, 0xD6BE60, void* A1, void* A2)
{
    if (!Configuration::compileShadersBeforeStarting || ShaderCache::compilingShaderCount == 0)
        originalInternalUpdate(A1, A2);
}

extern "C" __declspec(dllexport) void Init(ModInfo* info)
{
#if _DEBUG
    MessageBox(nullptr, TEXT("Attach to Debugger"), TEXT("GenerationsD3D11"), MB_ICONWARNING);
#endif

    std::string dir = info->CurrentMod->Path;

    size_t pos = dir.find_last_of("\\/");
    if (pos != std::string::npos)
        dir.erase(pos + 1);

    if (!Configuration::load(dir + "GenerationsD3D11.ini"))
        MessageBox(nullptr, TEXT("Unable to open \"GenerationsD3D11.ini\" in mod directory."), TEXT("GenerationsD3D11"), MB_ICONERROR);

    ShaderCache::directoryPath = dir;

#if _DEBUG
    INSTALL_HOOK(MyOutputDebugStringA);
    INSTALL_HOOK(MyOutputDebugStringW);

    if (!GetConsoleWindow())
        AllocConsole();

    freopen("CONOUT$", "w", stdout);
#endif
}

extern "C" __declspec(dllexport) void PostInit(ModInfo* info) // PostInit to prevent D3D9Ex conflict
{
    MemoryHandler::applyPatches();

    INSTALL_HOOK(LoadPictureData);
    INSTALL_HOOK(FillTexture);
    INSTALL_HOOK(Direct3DCreate);

    // Patch the window function to load the icon in the executable.
    // However, check whether any mods already wrote over this first.
    if (*reinterpret_cast<uint8_t*>(0xE7B843) == 0xFF)
    {
        WRITE_CALL(0xE7B843, LoadIconImpl);
        WRITE_NOP(0xE7B848, 1);
    }

    // Hide window when it's first created because it's not a pleasant sight to see it centered/resized afterwards.
    WRITE_MEMORY(0xE7B8F7, uint8_t, 0x00);

    // Prevent half-pixel correction
    WRITE_MEMORY(0x64F4C7, uintptr_t, 0x15C5858); // MTFx
    WRITE_MEMORY(0x64CC4B, uintptr_t, 0x15C5858); // ^^
    WRITE_MEMORY(0x7866E2, uintptr_t, 0x15C5858); // FxPipeline
    WRITE_MEMORY(0x64CBBB, uint8_t, 0xD9, 0xEE, 0x90, 0x90, 0x90, 0x90); // Devil's Details

    // Prevent conversion of restart indices to degenerate triangles
    WRITE_MEMORY(0x72EAEB, uint8_t, 0xEB);
    WRITE_MEMORY(0x736A18, uint8_t, 0xEB);
    WRITE_MEMORY(0x736B6B, uint8_t, 0xEB);
    WRITE_MEMORY(0x744B8B, uint8_t, 0xEB);
    WRITE_MEMORY(0x744E4B, uint8_t, 0xEB);
    WRITE_MEMORY(0x745027, uint8_t, 0xEB);

    // A big block of memory is allocated for degenerate triangles,
    // use the original index data instead

    WRITE_MEMORY(0x72EABC, uint8_t, 0x8B, 0x46, 0x08, 0x90, 0x90); // Move the original index data instead of allocating
    WRITE_NOP(0x72EB53, 5); // Nop the deallocation

    WRITE_MEMORY(0x7369EB, uint8_t, 0x8B, 0x43, 0x0C, 0x90, 0x90);
    WRITE_NOP(0x736AB4, 5);

    WRITE_MEMORY(0x736B3E, uint8_t, 0x8B, 0x47, 0x0C, 0x90, 0x90);
    WRITE_NOP(0x736C0E, 5);

    WRITE_MEMORY(0x744B5C, uint8_t, 0x8B, 0x47, 0x08, 0x90, 0x90);
    WRITE_NOP(0x744C41, 5);

    WRITE_MEMORY(0x744E1C, uint8_t, 0x8B, 0x47, 0x08, 0x90, 0x90);
    WRITE_NOP(0x744F01, 5);

    WRITE_MEMORY(0x744FF3, uint8_t, 0x8B, 0x43, 0x08, 0x90, 0x90);
    WRITE_NOP(0x7450D2, 5);

    // Smoke effect uses triangle fans, patch the index buffer to use triangles instead
    WRITE_MEMORY(0x11A379C, uint8_t, groundSmokeParticleIndexBufferSize);
    WRITE_MEMORY(0x11A37B0, uint8_t, groundSmokeParticleIndexBufferSize);
    WRITE_JUMP(0x11A37C0, groundSmokeParticleCopyIndexBufferMidAsmHook);
    WRITE_MEMORY(0x11A2C63, uint8_t, groundSmokeParticleIndexBufferCount / 3);
    WRITE_MEMORY(0x11A2C6D, uint8_t, D3DPT_TRIANGLELIST);

    // 30 FPS SFDs get decoded at the original frame rate, but get invalidated
    // at the game's frame rate. This is due to Lock/Unlock calls that
    // don't update the returned buffer, which causes flickering.
    
    // Patch Lock/Unlock calls to call BeginSfdDecodeCallback/EndSfdDecodeCallback instead.
    WRITE_MEMORY(0x5787C8, uint8_t, 0x64);
    WRITE_MEMORY(0x5787FA, uint8_t, 0x68);

    WRITE_MEMORY(0x583DF0, uint8_t, 0x64);
    WRITE_MEMORY(0x583E24, uint8_t, 0x68);

    WRITE_MEMORY(0xB1F184, uint8_t, 0x64);
    WRITE_MEMORY(0xB1F1CF, uint8_t, 0x68);

    WRITE_MEMORY(0x1058FC6, uint8_t, 0x64);
    WRITE_MEMORY(0x1059001, uint8_t, 0x68);

    WRITE_MEMORY(0x1059F92, uint8_t, 0x64);
    WRITE_MEMORY(0x1059FB1, uint8_t, 0x68);

    // Patch SFD decode function to copy data when it's valid.
    WRITE_JUMP(0x1059130, sfdDecodeTrampoline);

    // Don't update when compiling shaders at startup
    if (Configuration::compileShadersBeforeStarting)
        INSTALL_HOOK(InternalUpdate);

    // Sonic Team felt silly and decided to do GPU readback
    // to compute the AABB of Havok meshes when rendering them.
    // I'm not going to support this so just set the bounding sphere
    // to infinite from origin.
    WRITE_MEMORY(0x11AF3C2, uint8_t, 0xEB);

    // Disable unnecessary sky rendering that angers the debug layer.
    WRITE_MEMORY(0x13DDB20, uint32_t, 0);
}