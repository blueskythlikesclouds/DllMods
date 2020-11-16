#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/Type/hhSharedString.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Database
{
    class CDatabaseData : public Base::CObject
    {
    public:
        uint8_t m_Flags;
        Base::CSharedString m_TypeAndName;
    };

    ASSERT_OFFSETOF(CDatabaseData, m_Flags, 0x4);
    ASSERT_OFFSETOF(CDatabaseData, m_TypeAndName, 0x8);
    ASSERT_SIZEOF(CDatabaseData, 0xC);
}