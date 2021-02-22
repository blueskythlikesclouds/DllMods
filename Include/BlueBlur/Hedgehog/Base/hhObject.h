#pragma once

#include <BlueBlur.h>

namespace Hedgehog::Base
{
    class CObject
    {
    public:
        virtual ~CObject() = default;

        template<typename T>
        bool IsOfType()
        {
            return *(void**)this == T::ms_pVfTable;
        }
    };

    ASSERT_SIZEOF(CObject, 0x4);
}