#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Mirage
{
    class CCamera : public Base::CObject
    {
    public:
        virtual ~CCamera() = default;

        Math::CMatrix m_View;
        Math::CMatrix44 m_Projection;
        Math::CVector m_Position;
        Math::CVector m_Direction;
        float m_AspectRatio;
        float m_Near;
        float m_Far;
    };

    ASSERT_OFFSETOF(CCamera, m_View, 0x10);
    ASSERT_OFFSETOF(CCamera, m_Projection, 0x50);
    ASSERT_OFFSETOF(CCamera, m_Position, 0x90);
    ASSERT_OFFSETOF(CCamera, m_Direction, 0xA0);
    ASSERT_OFFSETOF(CCamera, m_AspectRatio, 0xB0);
    ASSERT_OFFSETOF(CCamera, m_Near, 0xB4);
    ASSERT_OFFSETOF(CCamera, m_Far, 0xB8);
    ASSERT_SIZEOF(CCamera, 0xC0);
}