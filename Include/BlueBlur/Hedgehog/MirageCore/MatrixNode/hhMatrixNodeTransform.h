#pragma once

#include <Hedgehog/MirageCore/MatrixNode/hhMatrixNode.h>

namespace Hedgehog::Mirage
{
    class CMatrixNodeTransform : public CMatrixNode
    {
    public:
        Math::CQuaternion m_Rotation;
        Math::CVector m_Position;
        Math::CMatrix m_Transform;
        Math::CMatrix m_PrevTransform;
    };

    ASSERT_OFFSETOF(CMatrixNodeTransform, m_Rotation, 0x60);
    ASSERT_OFFSETOF(CMatrixNodeTransform, m_Position, 0x70);
    ASSERT_OFFSETOF(CMatrixNodeTransform, m_Transform, 0x80);
    ASSERT_OFFSETOF(CMatrixNodeTransform, m_PrevTransform, 0xC0);
    ASSERT_SIZEOF(CMatrixNodeTransform, 0x100);
}