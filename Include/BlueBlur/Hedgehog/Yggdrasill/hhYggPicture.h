#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Yggdrasill/hhYggTexture.h>

namespace Hedgehog::Mirage
{
    class CPictureData;
}

namespace Hedgehog::Yggdrasill
{
    class CYggPicture : public CYggTexture
    {
    public:
        boost::shared_ptr<Mirage::CPictureData> m_spPictureData;
        INSERT_PADDING(0x8);
    };

    ASSERT_OFFSETOF(CYggPicture, m_spPictureData, 0xC0);
    ASSERT_SIZEOF(CYggPicture, 0xD0);
}
