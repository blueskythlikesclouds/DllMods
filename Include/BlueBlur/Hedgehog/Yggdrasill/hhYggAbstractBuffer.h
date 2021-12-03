#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/Type/hhSharedString.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Yggdrasill
{
    class CYggScheduler;

    class CYggAbstractBuffer : public Base::CObject
    {
    public:
        INSERT_PADDING(0x8);
        CYggScheduler* m_pScheduler;
        Base::CSharedString m_Name;
        INSERT_PADDING(0x1);
        bool m_AutoReset;
        INSERT_PADDING(0x6);

        virtual ~CYggAbstractBuffer() = default;
        virtual void _4() = 0;
        virtual void _8() = 0;
        virtual void _C() = 0;
    };

    ASSERT_OFFSETOF(CYggAbstractBuffer, m_pScheduler, 0xC);
    ASSERT_OFFSETOF(CYggAbstractBuffer, m_Name, 0x10);
    ASSERT_OFFSETOF(CYggAbstractBuffer, m_AutoReset, 0x15);
    ASSERT_SIZEOF(CYggAbstractBuffer, 0x1C);
}