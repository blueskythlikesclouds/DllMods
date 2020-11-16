#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Base
{
    class CSharedString;
}

namespace Hedgehog::Database
{
    class CDatabase;
    class CRawData;

    static FUNCTION_PTR(void, __thiscall, fpCDatabaseGetRawData, 0x40F010,
        CDatabase* This, boost::shared_ptr<CRawData>& spRawData, const Hedgehog::Base::CSharedString& name, uint32_t unknown);

    class CDatabase : public Base::CObject
    {
    public:
        INSERT_PADDING(0xEC);

        void GetRawData(boost::shared_ptr<CRawData>& spRawData, const Base::CSharedString& name, uint32_t unknown)
        {
            fpCDatabaseGetRawData(this, spRawData, name, unknown);
        }
    };

    ASSERT_SIZEOF(CDatabase, 0xF0);
}