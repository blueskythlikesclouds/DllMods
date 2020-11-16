#pragma once

#include <BlueBlur.h>

namespace Hedgehog::Base
{
    class CObject
    {
    public:
        virtual ~CObject() = 0;
    };

    ASSERT_SIZEOF(CObject, 0x4);
}