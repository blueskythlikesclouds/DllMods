#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Mirage
{
    class CStaticLightContext;

    class CLightManager : public Base::CObject
    {
    public:
        virtual ~CLightManager() = default;

        INSERT_PADDING(0xB0);
        Math::CVector m_GlobalLightDiffuse;
        Math::CVector m_GlobalLightSpecular;
        Math::CVector m_GlobalLightDirection;
        INSERT_PADDING(0x54);
        CStaticLightContext* m_pStaticLightContext;
        INSERT_PADDING(0x28);
    };

    ASSERT_OFFSETOF(CLightManager, m_GlobalLightDiffuse, 0xC0);
    ASSERT_OFFSETOF(CLightManager, m_GlobalLightSpecular, 0xD0);
    ASSERT_OFFSETOF(CLightManager, m_GlobalLightDirection, 0xE0);
    ASSERT_OFFSETOF(CLightManager, m_pStaticLightContext, 0x144);
    ASSERT_SIZEOF(CLightManager, 0x170);
}