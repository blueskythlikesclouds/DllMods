#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Mirage
{
    class CCamera : public Base::CObject
    {
    public:
        INSERT_PADDING(0xC);
        Eigen::Matrix4f m_View;
        Eigen::Matrix4f m_Projection;
        Eigen::Vector3f m_Position;
        INSERT_PADDING(0x4);
        Eigen::Vector3f m_Direction;
        INSERT_PADDING(0x4);
        float m_AspectRatio;
        float m_Near;
        float m_Far;
        INSERT_PADDING(0x4);
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