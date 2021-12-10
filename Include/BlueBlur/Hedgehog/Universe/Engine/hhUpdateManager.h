#pragma once

#include <BlueBlur.h>

namespace Hedgehog::Base
{
    class CSharedString;
}

namespace Hedgehog::Universe
{
    class CUpdateUnit;
    class CUpdateManager;

    static FUNCTION_PTR(void, __thiscall, fpCUpdateManagerAdd, 0x77AF20,
        CUpdateManager* This, const Hedgehog::Base::CSharedString& category, CUpdateUnit* pUpdateUnit);

    class CUpdateManager
    {
    public:
        void Add(const Hedgehog::Base::CSharedString& category, CUpdateUnit* pUpdateUnit)
        {
            fpCUpdateManagerAdd(this, category, pUpdateUnit);
        }
    };
}