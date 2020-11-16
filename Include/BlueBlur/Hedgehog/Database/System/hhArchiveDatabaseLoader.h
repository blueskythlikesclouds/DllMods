#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Database/System/hhDatabaseLoader.h>

namespace Hedgehog::Database
{
    class CArchiveDatabaseLoader;

    static FUNCTION_PTR(void, __thiscall, fpCArchiveDatabaseLoaderLoad, 0x6A1A80,
        CArchiveDatabaseLoader* This, const boost::shared_ptr<CDatabase>& spDatabase, boost::shared_ptr<uint8_t[]> spData, uint32_t dataSize, uint32_t _dataSize, void* pFileReader);

    class CArchiveDatabaseLoader : public CDatabaseLoader
    {
    public:
        INSERT_PADDING(0x8);
    };

    ASSERT_SIZEOF(CArchiveDatabaseLoader, 0x7C);
}
