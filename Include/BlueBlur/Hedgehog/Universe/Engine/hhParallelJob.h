#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Universe/Engine/hhUpdateInfo.h>

namespace Hedgehog::Universe
{
    class IParallelJob
    {
    public:
        virtual ~IParallelJob() = default;

        virtual void ExecuteParallelJob(const SUpdateInfo& updateInfo) = 0;
    };

    ASSERT_SIZEOF(IParallelJob, 0x4);
}