#pragma once

#include <BlueBlur.h>
#include <Sonic/Tool/ParameterEditor/AbstractParameter.h>

namespace Sonic
{
    class CAbstractParameterNode : public CAbstractParameter
    {
    public:
        INSERT_PADDING(0x8);
    };

    ASSERT_SIZEOF(CAbstractParameterNode, 0x3C);
}