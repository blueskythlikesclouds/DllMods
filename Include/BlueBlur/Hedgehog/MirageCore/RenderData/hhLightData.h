#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Database/System/hhDatabaseData.h>

namespace Hedgehog::Mirage
{
    enum ELightType : uint32_t
    {
        eLightType_Directional = 0,
        eLightType_Point = 1
    };

    class CLightData : public Hedgehog::Database::CDatabaseData
    {
    public:
        Math::CVector m_Position;
        Math::CVector4 m_Color;
        Math::CVector4 m_Range;
        Math::CVector m_Direction;
        ELightType m_Type;
        uint32_t m_Attribute;
        INSERT_PADDING(0x8);
    };

    ASSERT_OFFSETOF(CLightData, m_Position, 0x10);
    ASSERT_OFFSETOF(CLightData, m_Color, 0x20);
    ASSERT_OFFSETOF(CLightData, m_Range, 0x30);
    ASSERT_OFFSETOF(CLightData, m_Direction, 0x40);
    ASSERT_OFFSETOF(CLightData, m_Type, 0x50);
    ASSERT_OFFSETOF(CLightData, m_Attribute, 0x54);
    ASSERT_SIZEOF(CLightData, 0x60);
}
