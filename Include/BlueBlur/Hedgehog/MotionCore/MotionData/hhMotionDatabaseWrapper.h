#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/Type/hhSharedString.h>

namespace Hedgehog::Database
{
    class CDatabase;
}

namespace Hedgehog::Motion
{
    class CLightMotionData;
    class CMotionDatabaseWrapper;

    static FUNCTION_PTR(void, __thiscall, fpCDatabaseGetLightMotionData, 0x7599C0,
        CMotionDatabaseWrapper* This, boost::shared_ptr<CLightMotionData>& spLightMotionData, const Hedgehog::Base::CSharedString& name, uint32_t unknown);

    class CMotionDatabaseWrapper
    {
    public:
        Hedgehog::Database::CDatabase* m_pDatabase;

        CMotionDatabaseWrapper(Hedgehog::Database::CDatabase* pDatabase) : m_pDatabase(pDatabase)
        {
        }

        void GetLightMotionData(boost::shared_ptr<CLightMotionData>& spLightMotionData, const Base::CSharedString& name, uint32_t unknown)
        {
            fpCDatabaseGetLightMotionData(this, spLightMotionData, name, unknown);
        }
    };

    ASSERT_OFFSETOF(CMotionDatabaseWrapper, m_pDatabase, 0x0);
    ASSERT_SIZEOF(CMotionDatabaseWrapper, 0x4);
}