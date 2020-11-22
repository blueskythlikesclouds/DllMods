#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhRefCountObject.h>

namespace Hedgehog::Mirage
{
    class CLightListData;

    class CStaticLightContext : public Base::CRefCountObject
    {
    public:
        INSERT_PADDING(0x20);
        boost::shared_ptr<CLightListData> m_spLightListData;
        INSERT_PADDING(0x4);
        uint32_t m_LightCount;
        INSERT_PADDING(0x8);
    };

    ASSERT_OFFSETOF(CStaticLightContext, m_spLightListData, 0x28);
    ASSERT_OFFSETOF(CStaticLightContext, m_LightCount, 0x34);
    ASSERT_SIZEOF(CStaticLightContext, 0x40);
}
