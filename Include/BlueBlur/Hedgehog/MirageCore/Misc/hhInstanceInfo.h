#pragma once

#include <BlueBlur.h>
#include <Hedgehog/MirageCore/MatrixNode/hhMatrixNodeListener.h>

namespace Hedgehog::Mirage
{
    class CInstanceInfo : public CMatrixNodeListener
    {
    public:
        INSERT_PADDING(0x24C);
        Math::CMatrix m_Transform;
        INSERT_PADDING(0xB0);
    };

    ASSERT_OFFSETOF(CInstanceInfo, m_Transform, 0x250);
    ASSERT_SIZEOF(CInstanceInfo, 0x340);
}