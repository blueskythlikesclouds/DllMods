#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/hhObject.h>
#include <Hedgehog/Base/Container/hhVector.h>
#include <Hedgehog/Base/Type/hhSharedString.h>

namespace Sonic
{
    class CEditParam;

    class CAbstractParameter : public Hedgehog::Base::CObject
    {
    public:
        CEditParam* m_pEditParam;
        Hedgehog::Base::CSharedString m_Name;
        Hedgehog::Base::CSharedString m_Description;
        hh::vector<boost::shared_ptr<CAbstractParameter>> m_Children;
        INSERT_PADDING(0x14);
    };

    ASSERT_OFFSETOF(CAbstractParameter, m_pEditParam, 0x4);
    ASSERT_OFFSETOF(CAbstractParameter, m_Name, 0x8);
    ASSERT_OFFSETOF(CAbstractParameter, m_Description, 0xC);
    ASSERT_OFFSETOF(CAbstractParameter, m_Children, 0x10);
    ASSERT_SIZEOF(CAbstractParameter, 0x34);
}