#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Yggdrasill
{
    class CYggAllocator : public Base::CObject
    {
    public:
        INSERT_PADDING(0x1C);

        virtual ~CYggAllocator() = default;
    };

    ASSERT_SIZEOF(CYggAllocator, 0x20);
}