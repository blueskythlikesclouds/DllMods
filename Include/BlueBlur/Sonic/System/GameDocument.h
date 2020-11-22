#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/Container/hhList.h>
#include <Hedgehog/Base/Thread/hhSynchronizedObject.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Database
{
    class CDatabase;
}

namespace Sonic
{
    class CGameObject;
    class CLightManager;

    class CGameDocument : public Hedgehog::Base::CObject, public Hedgehog::Base::CSynchronizedObject
    {
    public:
        class CMember
        {
        public:
            INSERT_PADDING(0x1C);
            boost::shared_ptr<Hedgehog::Database::CDatabase> m_spDatabase;
            INSERT_PADDING(0x4);
            hh::list<boost::shared_ptr<CGameObject>> m_GameObjects;
            INSERT_PADDING(0x50);
            boost::shared_ptr<CLightManager> m_spLightManager;
            INSERT_PADDING(0x128);
        };

        static constexpr CGameDocument** ms_pInstance = (CGameDocument**)0x1E0BE5C;

        CMember* m_pMember;
        INSERT_PADDING(0x14);

        virtual void _4() = 0;
        virtual void _8() = 0;
        virtual void _C() = 0;
        virtual void _10() = 0;
        virtual void _14() = 0;
    };

    ASSERT_OFFSETOF(CGameDocument::CMember, m_spDatabase, 0x1C);
    ASSERT_OFFSETOF(CGameDocument::CMember, m_GameObjects, 0x28);
    ASSERT_OFFSETOF(CGameDocument::CMember, m_spLightManager, 0x80);
    ASSERT_SIZEOF(CGameDocument::CMember, 0x1B0);

    ASSERT_OFFSETOF(CGameDocument, m_pCriticalSection, 0x4);
    ASSERT_OFFSETOF(CGameDocument, m_pMember, 0x8);
    ASSERT_SIZEOF(CGameDocument, 0x20);
}