#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Mirage
{
    class CDirector : public Hedgehog::Base::CObject
    {
    };

    ASSERT_SIZEOF(CDirector, 0x4);
}
