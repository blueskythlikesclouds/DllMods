#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Universe/Engine/hhParallelJob.h>

namespace Hedgehog::Universe
{
    class CUpdateUnit : public IParallelJob
    {
    public:
        INSERT_PADDING(0x24);

        virtual void _8() = 0;
        virtual void _C() = 0;
    };

    ASSERT_SIZEOF(CUpdateUnit, 0x28);
}
