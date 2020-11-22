#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Universe
{
    class IParallelJob : public Base::CObject
    {
    public:
        virtual void _4() = 0;
    };

    ASSERT_SIZEOF(IParallelJob, 0x4);
}