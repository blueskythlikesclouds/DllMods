#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/Type/hhSharedString.h>
#include <Hedgehog/Base/Thread/hhSynchronizedObject.h>
#include <Hedgehog/Universe/Engine/hhMessageActor.h>

namespace Hedgehog::Mirage
{
    class CRenderScene;
}

namespace Sonic
{
    class CCamera;

    class CWorld : public Hedgehog::Base::CSynchronizedObject, public Hedgehog::Universe::CMessageActor
    {
    public:
        class CMember
        {
        public:
            boost::shared_ptr<Hedgehog::Mirage::CRenderScene> m_spRenderScene;
            Hedgehog::Base::CSharedString m_Name;
            boost::shared_ptr<CCamera> m_spCamera;
            boost::shared_ptr<CCamera> m_spOverrideCamera;
            INSERT_PADDING(0x64);
        };

        CMember* m_pMember;

        boost::shared_ptr<CCamera> GetCamera() const
        {
            return m_pMember->m_spOverrideCamera ? m_pMember->m_spOverrideCamera : m_pMember->m_spCamera;
        }
    };

    ASSERT_OFFSETOF(CWorld::CMember, m_spRenderScene, 0x0);
    ASSERT_OFFSETOF(CWorld::CMember, m_Name, 0x8);
    ASSERT_OFFSETOF(CWorld::CMember, m_spCamera, 0xC);
    ASSERT_OFFSETOF(CWorld::CMember, m_spOverrideCamera, 0x14);
    ASSERT_SIZEOF(CWorld::CMember, 0x80);

    ASSERT_OFFSETOF(CWorld, m_pMember, 0x80);
    ASSERT_SIZEOF(CWorld, 0x84);
}