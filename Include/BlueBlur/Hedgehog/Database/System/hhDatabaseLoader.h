#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/hhObject.h>
#include <Hedgehog/Base/Type/hhSharedString.h>

namespace Hedgehog::Database
{
    class CDatabase;
    class CDatabaseLoader;

    static FUNCTION_PTR(void, __thiscall, fpCDatabaseLoaderLoadData, 0x6999C0,
        CDatabaseLoader* This, const boost::shared_ptr<CDatabase>& spDatabase, const Base::CSharedString& name, 
        uint8_t* data, uint32_t dataSize, boost::shared_ptr<uint8_t[]> spDatabaseData, void* pFileReader);

    class CDatabaseLoader : public Base::CObject
    {
    public:
        INSERT_PADDING(0x70);

        virtual void Load(const boost::shared_ptr<CDatabase>& spDatabase, boost::shared_ptr<uint8_t[]> spData, uint32_t dataSize, uint32_t _dataSize, void* pFileReader) = 0;

        void LoadData(const boost::shared_ptr<CDatabase>& spDatabase, const Base::CSharedString& name,
            uint8_t* data, uint32_t dataSize, boost::shared_ptr<uint8_t[]> spDatabaseData, void* pFileReader)
        {
            fpCDatabaseLoaderLoadData(this, spDatabase, name, data, dataSize, std::move(spDatabaseData), pFileReader);
        }
    };

    ASSERT_SIZEOF(CDatabaseLoader, 0x74);
}
