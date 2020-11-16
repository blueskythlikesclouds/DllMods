#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Database/System/hhDatabaseData.h>

namespace Hedgehog::Database
{
    class CRawData : public CDatabaseData
    {
    public:
        uint32_t m_DataSize;
        INSERT_PADDING(0x4);
        boost::shared_ptr<uint8_t> m_spData;
    };

    ASSERT_OFFSETOF(CRawData, m_DataSize, 0xC);
    ASSERT_OFFSETOF(CRawData, m_spData, 0x14);
    ASSERT_SIZEOF(CRawData, 0x1C);
}