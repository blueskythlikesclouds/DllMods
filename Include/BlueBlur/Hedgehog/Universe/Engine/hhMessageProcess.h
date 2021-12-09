#pragma once

#include <BlueBlur.h>

namespace Hedgehog::Universe
{
    class IMessageProcess
    {
    public:
        // Can't replicate RTTI if I add any virtual functions (and an extra vtable gets added),
        // so this is going to stay empty.
        IMessageProcess() {}
        IMessageProcess(const null_ctor&) {}
    };
}