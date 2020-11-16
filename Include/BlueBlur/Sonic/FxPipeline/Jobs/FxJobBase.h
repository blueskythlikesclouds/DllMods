#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Yggdrasill/hhYggJob.h>

namespace Sonic
{
    class CFxJobBase : public Hedgehog::Yggdrasill::CYggJob
    {
    public:
        INSERT_PADDING(0x8);
    };

    ASSERT_SIZEOF(CFxJobBase, 0x48);
}