#pragma once

namespace Hedgehog::Universe
{
    class IMessageProcess
    {
        // Can't replicate RTTI if I add any virtual functions (and an extra vtable gets added),
        // so this is going to stay empty.
    };
}