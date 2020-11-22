#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/hhRefCountObject.h>
#include <Hedgehog/Base/Type/hhSharedString.h>

namespace Sonic
{
    class CParamBase : public Hedgehog::Base::CRefCountObject
    {
    public:
        INSERT_PADDING(0x4);
        Hedgehog::Base::CSharedString m_Name;
        INSERT_PADDING(0x4);
    };

    ASSERT_OFFSETOF(CParamBase, m_Name, 0xC);
    ASSERT_SIZEOF(CParamBase, 0x14);
}
