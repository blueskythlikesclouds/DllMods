#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Yggdrasill/hhYggAbstractBuffer.h>

namespace Hedgehog::Yggdrasill
{
    struct SYggTextureCreationParams
    {
        INSERT_PADDING(0x4);
        uint32_t Format;
        uint32_t Usage;
        uint32_t Width;
        uint32_t Height;
        INSERT_PADDING(0x4);
        uint32_t Levels;
        uint32_t Pool;
        INSERT_PADDING(0x8);
    };

    class CYggSurface;
    class CYggTexture;

    static FUNCTION_PTR(void, __thiscall, fpCYggTextureGetSurface, 0x78AE30,
        CYggTexture* This, boost::shared_ptr<CYggSurface>& spSurface, uint32_t mipIndex, uint32_t levelIndex);

    class CYggTexture : public CYggAbstractBuffer
    {
    public:
        DX_PATCH::IDirect3DBaseTexture9* m_pD3DTexture;
        INSERT_PADDING(0x10);
        SYggTextureCreationParams m_CreationParams;
        INSERT_PADDING(0x68);

        void GetSurface(boost::shared_ptr<CYggSurface>& spSurface, uint32_t mipIndex, uint32_t levelIndex)
        {
            fpCYggTextureGetSurface(this, spSurface, mipIndex, levelIndex);
        }
    };

    ASSERT_OFFSETOF(CYggTexture, m_pD3DTexture, 0x1C);
    ASSERT_OFFSETOF(CYggTexture, m_CreationParams, 0x30);
    ASSERT_SIZEOF(CYggTexture, 0xC0);
}
