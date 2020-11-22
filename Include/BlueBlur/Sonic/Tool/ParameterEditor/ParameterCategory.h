#pragma once

#include <BlueBlur.h>
#include <Sonic/Tool/ParameterEditor/AbstractParameterNode.h>

namespace Sonic
{
    class CParameterCategory : public CAbstractParameterNode
    {
    public:
        static constexpr void* ms_pVfTable = (void*)0x16E5FFC;

        INSERT_PADDING(0xC);
    };

    ASSERT_SIZEOF(CParameterCategory, 0x48);
}