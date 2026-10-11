#include "VolumeTexture.h"

VolumeTexture::VolumeTexture(Device* device, ID3D11Texture3D* texture) : BaseTexture(device, texture)
{
    texture->GetDesc(&desc);
    pixels.resize(desc.MipLevels);
    lockFlags.resize(desc.MipLevels, ~0u);
    device->get()->CreateShaderResourceView(texture, nullptr, srv.GetAddressOf());
}

HRESULT VolumeTexture::GetLevelDesc(UINT Level, D3DVOLUME_DESC* pDesc)
{
    if (!pDesc || Level >= desc.MipLevels)
        return D3DERR_INVALIDCALL;

    pDesc->Format = D3DFMT_A8R8G8B8;
    pDesc->Type = D3DRTYPE_VOLUME;
    pDesc->Usage = 0;
    pDesc->Pool = D3DPOOL_MANAGED;
    pDesc->Width = std::max(1u, desc.Width >> Level);
    pDesc->Height = std::max(1u, desc.Height >> Level);
    pDesc->Depth = std::max(1u, desc.Depth >> Level);
    return S_OK;
}

FUNCTION_STUB(HRESULT, VolumeTexture::GetVolumeLevel, UINT Level, IDirect3DVolume9** ppVolumeLevel)

HRESULT VolumeTexture::LockBox(UINT Level, D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags)
{
    if (!pLockedVolume || Level >= desc.MipLevels || lockFlags[Level] != ~0u)
        return D3DERR_INVALIDCALL;

    D3DVOLUME_DESC level;
    GetLevelDesc(Level, &level);
    if (pBox && (pBox->Left >= pBox->Right || pBox->Top >= pBox->Bottom || pBox->Front >= pBox->Back ||
        pBox->Right > level.Width || pBox->Bottom > level.Height || pBox->Back > level.Depth))
        return D3DERR_INVALIDCALL;

    pixels[Level].resize(static_cast<size_t>(level.Width) * level.Height * level.Depth);
    pLockedVolume->RowPitch = level.Width * sizeof(uint32_t);
    pLockedVolume->SlicePitch = pLockedVolume->RowPitch * level.Height;
    pLockedVolume->pBits = pixels[Level].data();
    if (pBox)
        pLockedVolume->pBits = pixels[Level].data() + (pBox->Front * level.Height + pBox->Top) * level.Width + pBox->Left;
    lockFlags[Level] = Flags;
    return S_OK;
}

HRESULT VolumeTexture::UnlockBox(UINT Level)
{
    if (Level >= desc.MipLevels || lockFlags[Level] == ~0u)
        return D3DERR_INVALIDCALL;

    if (!(lockFlags[Level] & D3DLOCK_READONLY))
    {
        // D3D11 volume textures use RGBA, so we use the D3D9 lock exposes BGRA pixels
        std::vector<uint32_t> rgba(pixels[Level].size());
        for (size_t i = 0; i < rgba.size(); i++)
        {
            uint32_t color = pixels[Level][i];
            rgba[i] = (color & 0xFF00FF00) | ((color & 0xFF) << 16) | ((color >> 16) & 0xFF);
        }
        D3DVOLUME_DESC level;
        GetLevelDesc(Level, &level);
        const auto lock = device->lock();
        device->getContext()->UpdateSubresource(resource.Get(), Level, nullptr, rgba.data(),
            level.Width * sizeof(uint32_t), level.Width * level.Height * sizeof(uint32_t));
    }
    lockFlags[Level] = ~0u;
    return S_OK;
}

HRESULT VolumeTexture::AddDirtyBox(const D3DBOX* pDirtyBox)
{
    return S_OK;
}
