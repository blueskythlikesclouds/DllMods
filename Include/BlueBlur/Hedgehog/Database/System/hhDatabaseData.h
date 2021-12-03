#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/Type/hhSharedString.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Database
{
    class CDatabaseData;

    static inline FUNCTION_PTR(bool, __thiscall, fpCDatabaseDataIsMadeAllInternal, 0x6993E0, CDatabaseData* This);

    enum EDatabaseDataFlags : uint8_t
    {
        eDatabaseDataFlags_IsMadeOne = 0x1,
        eDatabaseDataFlags_IsMadeAll = 0x2,
        eDatabaseDataFlags_CreatedFromArchive = 0x4,
        eDatabaseDataFlags_IsMadeMakingOne = 0x8
    };

    class CDatabaseData : public Base::CObject
    {
    public:
        uint8_t m_Flags; // see EDatabaseDataFlags
        Base::CSharedString m_TypeAndName;

        virtual ~CDatabaseData() = default;

        virtual bool CheckMadeAll()
        {
            return true;
        }

        bool IsMadeAllInternal()
        {
            // Alternatively can call fpCDatabaseDataIsMadeAllInternal
            if ((m_Flags & eDatabaseDataFlags_IsMadeOne) == 0 || !CheckMadeAll())
                return false;

            m_Flags |= eDatabaseDataFlags_IsMadeAll;
            return true;
        }

        bool IsMadeAll()
        {
            return (m_Flags & eDatabaseDataFlags_IsMadeAll) != 0 || IsMadeAllInternal();
        }
    };

    ASSERT_OFFSETOF(CDatabaseData, m_Flags, 0x4);
    ASSERT_OFFSETOF(CDatabaseData, m_TypeAndName, 0x8);
    ASSERT_SIZEOF(CDatabaseData, 0xC);
}