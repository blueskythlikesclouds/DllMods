#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Universe/Engine/hhParallelJob.h>

namespace Hedgehog::Universe
{
    class Message;

    class IMessageProcess : public IParallelJob
    {
    public:
        virtual void ProcessMessage(const Message& message, bool process) = 0;
    };

    ASSERT_SIZEOF(IMessageProcess, 0x4);
}