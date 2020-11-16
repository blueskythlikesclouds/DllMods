#pragma once

#include <BlueBlur.h>

namespace Hedgehog::Mirage
{
    class CRenderingInfrastructure;
}

namespace Hedgehog::Yggdrasill
{
    class CYggDevice;
    class CYggSceneRenderer;

    class CYggMisc
    {
    public:
        INSERT_PADDING(0x8);
        Mirage::CRenderingInfrastructure* m_pRenderingInfrastructure;
        INSERT_PADDING(0x4);
        CYggDevice* m_pDevice;
        boost::shared_ptr<CYggSceneRenderer> m_spSceneRenderer;
        INSERT_PADDING(0x28);
    };

    ASSERT_OFFSETOF(CYggMisc, m_pRenderingInfrastructure, 8);
    ASSERT_OFFSETOF(CYggMisc, m_pDevice, 0x10);
    ASSERT_OFFSETOF(CYggMisc, m_spSceneRenderer, 0x14);
    ASSERT_SIZEOF(CYggMisc, 0x44);
}
