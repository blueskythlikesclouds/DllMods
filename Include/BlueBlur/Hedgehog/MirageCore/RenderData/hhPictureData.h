#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Database/System/hhDatabaseData.h>

namespace Hedgehog::Mirage
{
    enum EPictureType : uint32_t
    {
        HH_MR_PICTURE_TYPE_INVALID = (uint32_t)-1,
        HH_MR_PICTURE_TYPE_TEXTURE = 0,
        HH_MR_PICTURE_TYPE_CUBE_TEXTURE = 1,
        HH_MR_PICTURE_TYPE_VOLUME_TEXTURE = 2
    };

    class CPictureData : public Database::CDatabaseData
    {
    public:
        DX_PATCH::IDirect3DBaseTexture9* m_pD3DTexture;
        EPictureType m_Type;
        INSERT_PADDING(0x8);
    };

    ASSERT_OFFSETOF(CPictureData, m_pD3DTexture, 0xC);
    ASSERT_OFFSETOF(CPictureData, m_Type, 0x10);
    ASSERT_SIZEOF(CPictureData, 0x1C);
}