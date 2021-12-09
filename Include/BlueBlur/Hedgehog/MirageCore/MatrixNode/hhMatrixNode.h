#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Mirage
{
    class CMatrixNode : public Base::CObject
    {
    public:
        INSERT_PADDING(0x5C);

        virtual ~CMatrixNode() = default;
    };

    ASSERT_SIZEOF(CMatrixNode, 0x60);
}