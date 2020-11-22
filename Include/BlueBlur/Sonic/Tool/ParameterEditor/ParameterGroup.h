#pragma once

#include <BlueBlur.h>
#include <Sonic/Tool/ParameterEditor/AbstractParameterNode.h>

namespace Sonic
{
    class CParameterGroup : public CAbstractParameterNode
    {
    public:
        static constexpr void* ms_pVfTable = (void*)0x16E5FA4;
    };

    ASSERT_SIZEOF(CParameterGroup, 0x3C);
}