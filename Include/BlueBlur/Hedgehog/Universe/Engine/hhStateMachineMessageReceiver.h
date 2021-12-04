#pragma once 

#include <BlueBlur.h>

namespace Hedgehog::Universe
{
    class Message;

    class IStateMachineMessageReceiver
    {
    public:
        virtual bool ReceiveMessage(const Message& message, bool flag) = 0;
    };

    ASSERT_SIZEOF(IStateMachineMessageReceiver, 4);
}