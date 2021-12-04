#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/Type/hhSharedString.h>

namespace Hedgehog::Universe
{
    struct SUpdateInfo
    {
        float ElapsedTime;
        uint32_t Frame;
        Base::CSharedString Category;
    };

    ASSERT_OFFSETOF(SUpdateInfo, ElapsedTime, 0x0);
    ASSERT_OFFSETOF(SUpdateInfo, Frame, 0x4);
    ASSERT_OFFSETOF(SUpdateInfo, Category, 0x8);
    ASSERT_SIZEOF(SUpdateInfo, 0xC);
}
