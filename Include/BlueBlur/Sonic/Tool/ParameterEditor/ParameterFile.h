#pragma once

#include <BlueBlur.h>
#include <Sonic/Tool/ParameterEditor/AbstractParameter.h>

namespace Sonic
{
    class CParameterFile : public CAbstractParameter
    {
    public:
        static constexpr void* ms_pVfTable = (void*)0x16E5E88;
    };

    ASSERT_SIZEOF(CParameterFile, 0x34);
}