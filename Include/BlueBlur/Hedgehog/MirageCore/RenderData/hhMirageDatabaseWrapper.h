#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/hhObject.h>
#include <Hedgehog/Base/Type/hhSharedString.h>

namespace Hedgehog::Database
{
    class CDatabase;
}

namespace Hedgehog::Mirage
{
    class CMirageDatabaseWrapper;
    class CPictureData;
    class CLightListData;

    static FUNCTION_PTR(CMirageDatabaseWrapper*, __thiscall, fpCMirageDatabaseWrapperCtor, 0x72EE30,
        CMirageDatabaseWrapper* This, Hedgehog::Database::CDatabase* pDatabase);

    static FUNCTION_PTR(void, __thiscall, fpCDatabaseGetPictureData, 0x72EFE0,
        CMirageDatabaseWrapper* This, boost::shared_ptr<CPictureData>& spPictureData, const Hedgehog::Base::CSharedString& name, uint32_t unknown);

    static FUNCTION_PTR(void, __thiscall, fpCDatabaseGetLightListData, 0x72F6F0,
        CMirageDatabaseWrapper* This, boost::shared_ptr<CLightListData>& spLightListData, const Hedgehog::Base::CSharedString& name, uint32_t unknown);

    class CMirageDatabaseWrapper : public Hedgehog::Base::CObject
    {
    public:
        Hedgehog::Database::CDatabase* m_pDatabase;
        bool m_Flag;

        CMirageDatabaseWrapper(Hedgehog::Database::CDatabase* pDatabase)
        {
            fpCMirageDatabaseWrapperCtor(this, pDatabase);
        }

        void GetPictureData(boost::shared_ptr<CPictureData>& spPictureData, const Base::CSharedString& name, uint32_t unknown)
        {
            fpCDatabaseGetPictureData(this, spPictureData, name, unknown);
        }

        void GetLightListData(boost::shared_ptr<CLightListData>& spLightListData, const Base::CSharedString& name, uint32_t unknown)
        {
            fpCDatabaseGetLightListData(this, spLightListData, name, unknown);
        }
    };

    ASSERT_OFFSETOF(CMirageDatabaseWrapper, m_pDatabase, 0x4);
    ASSERT_OFFSETOF(CMirageDatabaseWrapper, m_Flag, 0x8);
    ASSERT_SIZEOF(CMirageDatabaseWrapper, 0xC);
}