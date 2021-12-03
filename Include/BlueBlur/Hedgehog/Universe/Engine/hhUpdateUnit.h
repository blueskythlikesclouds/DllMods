#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Universe/Engine/hhParallelJob.h>

namespace Hedgehog::Universe
{
    class CUpdateUnit : public Base::CObject, public IParallelJob
    {
    public:
        INSERT_PADDING(0x24);

        virtual void ExecuteParallelJob(const SUpdateInfo& updateInfo) override
        {
            UpdateParallel(updateInfo);
        }

        virtual void UpdateParallel(const SUpdateInfo& updateInfo) {}
        virtual void _CUpdateUnitVTable0C() {}
    };

    ASSERT_SIZEOF(CUpdateUnit, 0x28);
}
