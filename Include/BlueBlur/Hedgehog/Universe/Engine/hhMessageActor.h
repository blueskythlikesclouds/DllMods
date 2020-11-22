#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Universe/Engine/hhMessageProcess.h>

namespace Hedgehog::Universe
{
    class CMessageActor : public IMessageProcess
    {
    public:
        INSERT_PADDING(0x78);
    };

    ASSERT_SIZEOF(CMessageActor, 0x7C);
}