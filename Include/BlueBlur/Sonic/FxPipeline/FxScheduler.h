#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Yggdrasill/hhYggScheduler.h>

namespace Sonic
{
    class CFxScheduler : public Hedgehog::Yggdrasill::CYggScheduler
    {
    public:
        INSERT_PADDING(0x88);
        float m_ElapsedTime;
    };

    ASSERT_OFFSETOF(CFxScheduler, m_ElapsedTime, 0x94);
    ASSERT_SIZEOF(CFxScheduler, 0x98);
}