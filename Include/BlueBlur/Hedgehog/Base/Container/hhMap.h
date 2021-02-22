#pragma once

#include <Hedgehog/Base/Container/hhTree.h>

namespace hh
{
    template<typename TKey, typename TValue>
    class pair
    {
    public:
        TKey m_Key;
        TValue m_Value;
    };

    template<typename TKey, typename TValue>
    class map : public tree<pair<TKey, TValue>>
    {
        
    };
}