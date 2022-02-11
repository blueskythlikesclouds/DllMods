#include "Direct3D9.h"
#include "Texture.h"
#include "Device.h"

HOOK(void, __cdecl, LoadPictureData, 0x743DE0,
    hh::mr::CPictureData* pPictureData, const uint8_t* pData, size_t length, hh::mr::CRenderingInfrastructure* pRenderingInfrastructure)
{
    if (pPictureData->m_Flags & hh::db::eDatabaseDataFlags_IsMadeOne)
        return;

    const ComPtr<Device> device = (Device*)pRenderingInfrastructure->m_RenderingDevice.m_pD3DDevice;

    ComPtr<ID3D12Resource> texture;
    std::vector<D3D12_SUBRESOURCE_DATA> subResources;

    const HRESULT result = DirectX::LoadDDSTextureFromMemory(device->getUnderlyingDevice(), pData, length, texture.GetAddressOf(), subResources);

    if (SUCCEEDED(result) && texture != nullptr)
    {
        // Create upload buffer.
        ComPtr<ID3D12Resource> uploadHeap;

        device->getUnderlyingDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(texture.Get(), 0, subResources.size())),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&uploadHeap));

        // Update subresources.
        auto& queue = device->getLoadQueue();
        const auto lock = queue.lock();

        UpdateSubresources(queue.getCommandList(), texture.Get(), uploadHeap.Get(), 0, 0, subResources.size(), subResources.data());

        // Transition the texture to a shader resource.
        queue.getCommandList()->ResourceBarrier(
            1,
            &CD3DX12_RESOURCE_BARRIER::Transition(
                texture.Get(),
                D3D12_RESOURCE_STATE_COPY_DEST,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

        // Execute command list before freeing upload heap.
        queue.executeCommandList();
        queue.waitForFenceEvent();
        queue.resetCommandList();

        // Free upload heap.
        uploadHeap.Reset();

        // Set resource name.
        WCHAR pictureName[0x100];
        MultiByteToWideChar(CP_UTF8, 0, pPictureData->m_TypeAndName.c_str() + 15, -1, pictureName, _countof(pictureName));
        texture->SetName(pictureName);

        pPictureData->m_pD3DTexture = (DX_PATCH::IDirect3DBaseTexture9*)(new Texture(device, texture));
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

HOOK(Direct3D9*, __cdecl, Direct3DCreate, 0xA5EDD0, UINT SDKVersion)
{
    return new Direct3D9(SDKVersion);
}

HOOK(void, WINAPI, MyOutputDebugStringA, &OutputDebugStringA, LPCSTR lpOutputString)
{
    printf(lpOutputString);
}

extern "C" __declspec(dllexport) void Init()
{
    INSTALL_HOOK(LoadPictureData);
    INSTALL_HOOK(FillTexture);
    INSTALL_HOOK(Direct3DCreate);

    WRITE_CALL(0x64CF9E, 0x64F470);

#if _DEBUG
    INSTALL_HOOK(MyOutputDebugStringA);

    if (!GetConsoleWindow())
        AllocConsole();

    freopen("CONOUT$", "w", stdout);

    WRITE_MEMORY(0xE7B8F7, uint8_t, 0x00);
#endif
}