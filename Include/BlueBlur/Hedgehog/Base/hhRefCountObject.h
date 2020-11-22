#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Base
{
    class CRefCountObject : public CObject
    {
    public:
        uint32_t m_RefCount;
    };

    ASSERT_OFFSETOF(CRefCountObject, m_RefCount, 0x4);
    ASSERT_SIZEOF(CRefCountObject, 0x8);
}