#pragma once

#include <BlueBlur.h>

namespace Hedgehog::Universe
{
    class Message;

    class IMessageProcess
    {
    public:
        virtual void ProcessMessage(const Message& message, bool process) = 0;
    };

    ASSERT_SIZEOF(IMessageProcess, 0x4);
}