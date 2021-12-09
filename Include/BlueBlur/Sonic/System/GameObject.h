#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/System/hhSymbol.h>
#include <Hedgehog/Universe/Engine/hhMessageActor.h>
#include <Hedgehog/Universe/Engine/hhUpdateUnit.h>

namespace Hedgehog::Mirage
{
    class CRenderable;
}

namespace Sonic
{
    class CGameObject;

    static uint32_t pCGameObjectCtor = 0xD601F0;

    static void fCGameObjectCtor(CGameObject* This)
    {
        __asm
        {
            mov edi, This
            call[pCGameObjectCtor]
        }
    }

    static FUNCTION_PTR(void, __thiscall, fpCGameObjectAddRenderable, 0xD5F620,
        CGameObject* This, const Hedgehog::Base::CStringSymbol category, const boost::shared_ptr<Hedgehog::Mirage::CRenderable>& spRenderable, const bool castShadow);

    class CGameObject : public Hedgehog::Universe::CUpdateUnit, public Hedgehog::Universe::CMessageActor
    {
    public:
        class CMember
        {
        public:
            INSERT_PADDING(0x80);
        };

        CMember* m_pMember;

        CGameObject(const null_ctor&) : CUpdateUnit(null_ctor{}), CMessageActor(null_ctor{}) {}

        CGameObject() : CGameObject(null_ctor{})
        {
            fCGameObjectCtor(this);
        }

        void AddRenderable(const Hedgehog::Base::CStringSymbol category, 
            const boost::shared_ptr<Hedgehog::Mirage::CRenderable>& spRenderable, const bool castShadow)
        {
            fpCGameObjectAddRenderable(this, category, spRenderable, castShadow);
        }
    };

    ASSERT_SIZEOF(CGameObject::CMember, 0x80);

    ASSERT_OFFSETOF(CGameObject, m_pMember, 0xA4);
    ASSERT_SIZEOF(CGameObject, 0xA8);
}
