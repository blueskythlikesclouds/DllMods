#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Universe/Engine/hhParallelJob.h>

namespace Hedgehog::Universe
{
    class CUpdateUnit;

    static FUNCTION_PTR(CUpdateUnit*, __thiscall, fpCUpdateUnitCtor, 0x76A950, CUpdateUnit* This);

    class CUpdateUnit : public Base::CObject, public IParallelJob
    {
    public:
        INSERT_PADDING(0x24);

        CUpdateUnit(const null_ctor&) : CObject(null_ctor{}), IParallelJob(null_ctor{}) {}

        CUpdateUnit() : CUpdateUnit(null_ctor{})
        {
            fpCUpdateUnitCtor(this);
        }

        virtual void ExecuteParallelJob(const SUpdateInfo& updateInfo) override
        {
            UpdateParallel(updateInfo);
        }

        virtual void UpdateParallel(const SUpdateInfo& updateInfo) {}
        virtual void _CUpdateUnitVTable0C(void*) {}
    };

    ASSERT_SIZEOF(CUpdateUnit, 0x28);
}
