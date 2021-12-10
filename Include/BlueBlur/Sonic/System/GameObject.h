#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/System/hhSymbol.h>
#include <Hedgehog/Base/Thread/hhHolder.h>
#include <Hedgehog/Universe/Engine/hhMessageActor.h>
#include <Hedgehog/Universe/Engine/hhUpdateUnit.h>

namespace Hedgehog::Database
{
    class CDatabase;
}

namespace Hedgehog::Mirage
{
    class CRenderable;
}

namespace Sonic
{
    class CGameDocument;
    class CGameObject;
    class CWorld;

    static uint32_t pCGameObjectCtor = 0xD601F0;

    static void fCGameObjectCtor(CGameObject* This)
    {
        __asm
        {
            mov edi, This
            call[pCGameObjectCtor]
        }
    }

    static FUNCTION_PTR(void, __thiscall, fpCGameObjectUpdateParallel, 0xD5F2A0,
        CGameObject* This, const Hedgehog::Universe::SUpdateInfo& updateInfo);

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

        virtual void UpdateParallel(const Hedgehog::Universe::SUpdateInfo& updateInfo)
        {
            fpCGameObjectUpdateParallel(this, updateInfo);
        }

        virtual bool _CGameObjectVTable10() { return true; }
        virtual void* _CGameObjectVTable14() { return 0; }

        virtual void Initialize(const Hedgehog::Base::THolder<CWorld>& worldHolder, 
            Sonic::CGameDocument* pGameDocument) {}

        virtual void AddCallback(const Hedgehog::Base::THolder<CWorld>& worldHolder, 
            Sonic::CGameDocument* pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase) = 0;

        virtual void RemoveCallback(Sonic::CGameDocument* pGameDocument) {}

        virtual void _CGameObjectVTable24(void*) {}
        virtual void _CGameObjectVTable28() {}
        virtual void _CGameObjectVTable2C(void*) {}
        virtual void _CGameObjectVTable30(void*) {}

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
