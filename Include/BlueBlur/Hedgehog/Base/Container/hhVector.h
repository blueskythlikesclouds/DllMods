#pragma once

#include <BlueBlur.h>

namespace hh
{
    template<typename T>
    class vector
    {
    public:
        INSERT_PADDING(0x4);
        T* m_pBegin;
        T* m_pEnd;
        T* m_pCapacityEnd;
    };
}