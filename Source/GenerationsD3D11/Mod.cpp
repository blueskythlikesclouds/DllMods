#include "Configuration.h"
#include "Device.h"
#include "D3D9.h"
#include "Texture.h"

#include "../GenerationsD3D9Ex/MemoryHandler.h"

HOOK(void, __cdecl, LoadPictureData, 0x743DE0,
    hh::mr::CPictureData* pPictureData, const uint8_t* pData, size_t length, hh::mr::CRenderingInfrastructure* pRenderingInfrastructure)
{
    if (pPictureData->m_Flags & hh::db::eDatabaseDataFlags_IsMadeOne)
        return;

    const ComPtr<Device> device = (Device*)pRenderingInfrastructure->m_RenderingDevice.m_pD3DDevice;

    ComPtr<ID3D11Resource> texture;
    ComPtr<ID3D11ShaderResourceView> srv;

    HRESULT hr;
    {
        const auto lock = device->lock();

        hr = DirectX::CreateDDSTextureFromMemory(device->get(), device->getContext(),
            pData, length, texture.GetAddressOf(), srv.GetAddressOf());
    }

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

HOOK(HRESULT, __stdcall, FillTexture, 0xA55270, Texture* texture, void* function, void* data)
{
    return S_OK;
}

HOOK(D3D9*, __cdecl, Direct3DCreate, 0xA5EDD0, UINT SDKVersion)
{
    return new D3D9(SDKVersion);
}

HOOK(void, WINAPI, MyOutputDebugStringA, &OutputDebugStringA, LPCSTR lpOutputString)
{
    printf(lpOutputString);
}

extern "C" __declspec(dllexport) void Init(ModInfo* info)
{
    std::string dir = info->CurrentMod->Path;

    size_t pos = dir.find_last_of("\\/");
    if (pos != std::string::npos)
        dir.erase(pos + 1);

    if (!Configuration::load(dir + "GenerationsD3D11.ini"))
        MessageBox(NULL, L"Failed to parse GenerationsD3D11.ini", NULL, MB_ICONERROR);

    // Hide window when it's first created because it's not a pleasant sight to see it centered/resized afterwards.
    WRITE_MEMORY(0xE7B8F7, uint8_t, 0x00);

    MemoryHandler::applyPatches();

    INSTALL_HOOK(LoadPictureData);
    INSTALL_HOOK(FillTexture);
    INSTALL_HOOK(Direct3DCreate);

    // Prevent half-pixel correction
    WRITE_MEMORY(0x64F4C7, uintptr_t, 0x15C5858); // MTFx
    WRITE_MEMORY(0x7866E2, uintptr_t, 0x15C5858); // FxPipeline

    // Ignore Devil's Details' fullscreen shader
    WRITE_CALL(0x64CF9E, 0x64F470);

    // Ignore SFD playback
    WRITE_MEMORY(0x7F2630, uint8_t, 0xC3);

#if _DEBUG
    INSTALL_HOOK(MyOutputDebugStringA);

    if (!GetConsoleWindow())
        AllocConsole();

    freopen("CONOUT$", "w", stdout);
#endif

    WRITE_MEMORY(0xE7B8F7, uint8_t, 0x00);
}