#include "DDSHandler.h"
#include "D3D9ExDevice.h"
#include "D3D9ExTexture.h"
#include "D3D9ExCubeTexture.h"
#include "D3D9ExVolumeTexture.h"

HOOK(void, __cdecl, LoadPictureData, 0x743DE0,
     Hedgehog::Mirage::CPictureData* pPictureData, const uint8_t* pData, size_t length, Hedgehog::Mirage::CRenderingInfrastructure* pRenderingInfrastructure)
{
    if (!pPictureData || (pPictureData->m_Flags & 1) != 0 || (pData && *(uint32_t*)pData != MAKEFOURCC('D', 'D', 'S', ' ')))
        return originalLoadPictureData(pPictureData, pData, length, pRenderingInfrastructure);

    D3D9ExDevice* const pDxpDevice = 
        (D3D9ExDevice*)pRenderingInfrastructure->m_RenderingDevice.m_pD3DDevice;

    IDirect3DBaseTexture9* pD3DBaseTexture = nullptr;
    const HRESULT result = DirectX::CreateDDSTextureFromMemory(pDxpDevice->GetD3DDevice(), pData, length, &pD3DBaseTexture);

    if (SUCCEEDED(result) && pD3DBaseTexture != nullptr)
    {
        D3D9ExBaseTexture* pDxpTexture = nullptr;

        switch (pD3DBaseTexture->GetType())
        {
        case D3DRTYPE_TEXTURE:
            pDxpTexture = new D3D9ExTexture(pDxpDevice, (IDirect3DTexture9*)pD3DBaseTexture);
            pPictureData->m_Type = Hedgehog::Mirage::ePictureType_Texture;
            break;

        case D3DRTYPE_CUBETEXTURE:
            pDxpTexture = new D3D9ExCubeTexture(pDxpDevice, (IDirect3DCubeTexture9*)pD3DBaseTexture);
            pPictureData->m_Type = Hedgehog::Mirage::ePictureType_CubeTexture;
            break;

        case D3DRTYPE_VOLUMETEXTURE:
            pDxpTexture = new D3D9ExVolumeTexture(pDxpDevice, (IDirect3DVolumeTexture9*)pD3DBaseTexture);
            pPictureData->m_Type = Hedgehog::Mirage::ePictureType_VolumeTexture;
            break;

        default:
            pPictureData->m_Type = Hedgehog::Mirage::ePictureType_Invalid;
            break;
        }

        pPictureData->m_pD3DTexture = (DX_PATCH::IDirect3DBaseTexture9*)pDxpTexture;
    }
    else
    {
        pPictureData->m_pD3DTexture = nullptr;
        pPictureData->m_Type = Hedgehog::Mirage::ePictureType_Invalid;
    }

    if (pD3DBaseTexture)
        pD3DBaseTexture->Release();

    pPictureData->m_Flags |= 1;
}

void DDSHandler::applyPatches()
{
    INSTALL_HOOK(LoadPictureData);
}
