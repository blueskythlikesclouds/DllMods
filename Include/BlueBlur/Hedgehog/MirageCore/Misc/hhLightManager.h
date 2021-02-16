#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Mirage
{
    class CStaticLightContext;

    class CLightManager : public Base::CObject
    {
    public:
        INSERT_PADDING(0xBC);
        Eigen::Vector3f m_GlobalLightDiffuse;
        INSERT_PADDING(0x4);
        Eigen::Vector3f m_GlobalLightSpecular;
        INSERT_PADDING(0x4);
        Eigen::Vector3f m_GlobalLightDirection;
        INSERT_PADDING(0x58);
        CStaticLightContext* m_pStaticLightContext;
        INSERT_PADDING(0x28);
    };

    ASSERT_OFFSETOF(CLightManager, m_GlobalLightDiffuse, 0xC0);
    ASSERT_OFFSETOF(CLightManager, m_GlobalLightSpecular, 0xD0);
    ASSERT_OFFSETOF(CLightManager, m_GlobalLightDirection, 0xE0);
    ASSERT_OFFSETOF(CLightManager, m_pStaticLightContext, 0x144);
    ASSERT_SIZEOF(CLightManager, 0x170);
}