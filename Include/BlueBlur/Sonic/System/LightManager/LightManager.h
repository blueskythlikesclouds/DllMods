#pragma once

#include <BlueBlur.h>

#include <Hedgehog/MirageCore/Misc/hhLightManager.h>
#include <Sonic/System/GameObject.h>

namespace Sonic
{
    class CLightManager : public CGameObject, public Hedgehog::Mirage::CLightManager
    {
    public:
        INSERT_PADDING(0x10);
    };

    ASSERT_SIZEOF(CLightManager, 0x230);
}