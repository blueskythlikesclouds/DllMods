#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/Container/hhVector.h>
#include <Hedgehog/Database/System/hhDatabaseData.h>

namespace Hedgehog::Mirage
{
    class CLightData;

    class CLightListData : public Hedgehog::Database::CDatabaseData
    {
    public:
        hh::vector<boost::shared_ptr<CLightData>> m_Lights;
    };

    ASSERT_OFFSETOF(CLightListData, m_Lights, 0xC);
    ASSERT_SIZEOF(CLightListData, 0x1C);
}
