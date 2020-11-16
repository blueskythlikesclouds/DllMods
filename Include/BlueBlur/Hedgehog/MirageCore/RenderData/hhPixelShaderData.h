#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Database/System/hhDatabaseData.h>

namespace Hedgehog::Mirage
{
    class CPixelShaderData : public Hedgehog::Database::CDatabaseData
    {
    public:
        INSERT_PADDING(0x18);
    };

    ASSERT_SIZEOF(CPixelShaderData, 0x24);
}
