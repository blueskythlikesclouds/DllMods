#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Database/System/hhDatabaseData.h>

namespace Hedgehog::Mirage
{
    class CVertexShaderData : public Database::CDatabaseData
    {
    public:
        INSERT_PADDING(0x18);
    };

    ASSERT_SIZEOF(CVertexShaderData, 0x24);
}
