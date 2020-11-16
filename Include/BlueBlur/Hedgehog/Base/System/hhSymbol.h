#pragma once

#include <BlueBlur.h>

namespace Hedgehog::Base
{
    struct SSymbolNode;

    static FUNCTION_PTR(SSymbolNode*, __cdecl, MakeStringSymbol, 0x6643D0, const char* pName);

    class CStringSymbol
    {
    public:
        SSymbolNode* m_pSymbolNode;

        CStringSymbol(const char* pName)
        {
            m_pSymbolNode = MakeStringSymbol(pName);
        }
    };

    ASSERT_OFFSETOF(CStringSymbol, m_pSymbolNode, 0);
    ASSERT_SIZEOF(CStringSymbol, 4);
}