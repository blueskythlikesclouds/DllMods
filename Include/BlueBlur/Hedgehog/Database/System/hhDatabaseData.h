#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/Type/hhSharedString.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Database
{
    class CDatabaseData;

    static inline FUNCTION_PTR(bool, __thiscall, fpCDatabaseDataValidate, 0x6993E0, CDatabaseData* This);

    class CDatabaseData : public Base::CObject
    {
    public:
        uint8_t m_Flags;
        Base::CSharedString m_TypeAndName;

        void Validate()
        {
            fpCDatabaseDataValidate(this);
        }
    };

    ASSERT_OFFSETOF(CDatabaseData, m_Flags, 0x4);
    ASSERT_OFFSETOF(CDatabaseData, m_TypeAndName, 0x8);
    ASSERT_SIZEOF(CDatabaseData, 0xC);
}