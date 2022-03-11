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

    const ComPtr<Device> device = (Device*)pRenderingInfrastructure->m_RenderingDevice.m_pD3DDevice;

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

        pPictureData->m_pD3DTexture = (DX_PATCH::IDirect3DBaseTexture9*)(new Texture(device.Get(), texture.Get(), srv.Get()));
        pPictureData->m_Type = hh::mr::ePictureType_Texture;
    }
    else
    {
        pPictureData->m_pD3DTexture = nullptr;
        pPictureData->m_Type = hh::mr::ePictureType_Invalid;
    }

    pPictureData->m_Flags |= hh::db::eDatabaseDataFlags_IsMadeOne;
}

HOOK(void, __fastcall, GameplayFlowDebugInitExit, 0xD0BA70, void* This)
{
    originalGameplayFlowDebugInitExit(This);

    ShaderCache::save();
    ShaderCache::clean();
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

HOOK(D3D9*, __cdecl, Direct3DCreate, 0xA5EDD0, UINT SDKVersion)
{
    return new D3D9(SDKVersion);
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
        MessageBox(NULL, L"Failed to parse GenerationsD3D11.ini", NULL, MB_ICONERROR);

    ShaderCache::init(dir);
    ShaderCache::load();

    MemoryHandler::applyPatches();

    INSTALL_HOOK(LoadPictureData);
    INSTALL_HOOK(GameplayFlowDebugInitExit);
    INSTALL_HOOK(FillTexture);
    INSTALL_HOOK(Direct3DCreate);

    // Hide window when it's first created because it's not a pleasant sight to see it centered/resized afterwards.
    WRITE_MEMORY(0xE7B8F7, uint8_t, 0x00);

    // Prevent half-pixel correction
    WRITE_MEMORY(0x64F4C7, uintptr_t, 0x15C5858); // MTFx
    WRITE_MEMORY(0x64CC4B, uintptr_t, 0x15C5858); // ^^
    WRITE_MEMORY(0x7866E2, uintptr_t, 0x15C5858); // FxPipeline
    WRITE_MEMORY(0x64CBBB, uint8_t, 0xD9, 0xEE, 0x90, 0x90, 0x90, 0x90); // Devil's Details

#if _DEBUG
    INSTALL_HOOK(MyOutputDebugStringA);
    INSTALL_HOOK(MyOutputDebugStringW);

    if (!GetConsoleWindow())
        AllocConsole();

    freopen("CONOUT$", "w", stdout);
#endif
}