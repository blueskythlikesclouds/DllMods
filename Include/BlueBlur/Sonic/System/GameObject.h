#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Universe/Engine/hhMessageActor.h>
#include <Hedgehog/Universe/Engine/hhUpdateUnit.h>

namespace Sonic
{
    class CGameObject : public Hedgehog::Universe::CUpdateUnit, public Hedgehog::Universe::CMessageActor
    {
    public:
        INSERT_PADDING(0xC);
    };

    ASSERT_SIZEOF(CGameObject, 0xB0);
}
