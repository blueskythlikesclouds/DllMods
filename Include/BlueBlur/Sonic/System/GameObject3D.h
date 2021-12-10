#pragma once

#include <BlueBlur.h>

#include <Hedgehog/MirageCore/MatrixNode/hhMatrixNodeListener.h>
#include <Sonic/System/GameObject.h>

namespace Hedgehog::Mirage
{
    class CMatrixNodeTransform;
}

namespace Sonic
{
    class CGameObject3D;

    static FUNCTION_PTR(CGameObject3D*, __stdcall, fpCGameObject3DCtor, 0xD5DAC0, CGameObject3D* This);

    static FUNCTION_PTR(bool, __thiscall, fpCGameObject3DMatrixNodeUpdatedCallback, 0xD5C780,
        CGameObject3D* This, const Hedgehog::Math::CMatrix& matrix, size_t flags);

    static FUNCTION_PTR(void, __stdcall, fpCGameObject3DSetPosition, 0xD5CE10,
        CGameObject3D* This, const Hedgehog::Math::CVector& position);

    class CGameObject3D : public CGameObject, public Hedgehog::Mirage::CMatrixNodeListener
    {
    public:
        INSERT_PADDING(0x8);
        boost::shared_ptr<Hedgehog::Mirage::CMatrixNodeTransform> m_spMatrixNodeTransform;
        INSERT_PADDING(0x34);

        CGameObject3D(const null_ctor&) : CGameObject(null_ctor{}), CMatrixNodeListener(null_ctor{}) {}

        CGameObject3D() : CGameObject3D(null_ctor{})
        {
            fpCGameObject3DCtor(this);
        }

        virtual void AddCallback(const Hedgehog::Base::THolder<CWorld>& worldHolder,
            Sonic::CGameDocument* pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase) override {}

        virtual bool MatrixNodeUpdatedCallback(const Hedgehog::Math::CMatrix& matrix, size_t flags) override
        {
            return fpCGameObject3DMatrixNodeUpdatedCallback(this, matrix, flags);
        }

        void SetPosition(const Hedgehog::Math::CVector& position)
        {
            fpCGameObject3DSetPosition(this, position);
        }
    };

    ASSERT_OFFSETOF(CGameObject3D, m_spMatrixNodeTransform, 0xB8);
    ASSERT_SIZEOF(CGameObject3D, 0xF4);
}