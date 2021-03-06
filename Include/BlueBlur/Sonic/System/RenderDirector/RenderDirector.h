#pragma once

#include <BlueBlur.h>

#include <Hedgehog/MirageCore/Misc/hhDirector.h>
#include <Sonic/System/GameObject.h>

namespace Sonic
{
    class CRenderDirector : public Hedgehog::Mirage::CDirector, public CGameObject
    {
    public:
        INSERT_PADDING(0x14);
    };

    ASSERT_SIZEOF(CRenderDirector, 0xC0);
}
