#pragma once 

#include <BlueBlur.h>

namespace Hedgehog::Universe
{
    class IStateMachineMessageReceiver
    {
    public:
        virtual void _0() = 0;
    };

    ASSERT_SIZEOF(IStateMachineMessageReceiver, 4);
}