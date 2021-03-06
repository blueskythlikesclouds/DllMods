#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/Container/hhVector.h>

namespace Hedgehog::Mirage
{
    class CLightData;
}

namespace Sonic
{
    class CLocalLight
    {
    public:
        boost::shared_ptr<Hedgehog::Mirage::CLightData> m_spLightData;
        INSERT_PADDING(0x8);
    };

    ASSERT_OFFSETOF(CLocalLight, m_spLightData, 0x0);
    ASSERT_SIZEOF(CLocalLight, 0x10);

    class CLocalLightContext
    {
    public:
        INSERT_PADDING(0x8);
        hh::vector<CLocalLight*> m_LocalLights;
    };

    ASSERT_OFFSETOF(CLocalLightContext, m_LocalLights, 0x8);
    ASSERT_SIZEOF(CLocalLightContext, 0x18);
}