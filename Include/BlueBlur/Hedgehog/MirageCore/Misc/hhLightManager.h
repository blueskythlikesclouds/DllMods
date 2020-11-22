#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Mirage
{
    class CStaticLightContext;

    class CLightManager : public Base::CObject
    {
    public:
        INSERT_PADDING(0x140);
        CStaticLightContext* m_pStaticLightContext;
        INSERT_PADDING(0x28);
    };

    ASSERT_OFFSETOF(CLightManager, m_pStaticLightContext, 0x144);
    ASSERT_SIZEOF(CLightManager, 0x170);
}