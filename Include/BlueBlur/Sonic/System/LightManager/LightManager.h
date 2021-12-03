#pragma once

#include <BlueBlur.h>

#include <Hedgehog/MirageCore/Misc/hhLightManager.h>
#include <Sonic/System/GameObject.h>

namespace Sonic
{
    class CLocalLightContext;

    class CLightManager : public CGameObject, public Hedgehog::Mirage::CLightManager
    {
    public:
        CLocalLightContext* m_pLocalLightContext;
        INSERT_PADDING(0xC);
    };

    ASSERT_OFFSETOF(CLightManager, m_pLocalLightContext, 0x220);
    ASSERT_SIZEOF(CLightManager, 0x230);
}