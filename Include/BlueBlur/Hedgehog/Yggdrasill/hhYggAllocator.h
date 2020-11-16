#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Yggdrasill
{
    class CYggAllocator : public Base::CObject
    {
    public:
        INSERT_PADDING(0x1C);
    };

    ASSERT_SIZEOF(CYggAllocator, 0x20);
}