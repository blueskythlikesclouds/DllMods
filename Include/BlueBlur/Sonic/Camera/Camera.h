#pragma once

#include <BlueBlur.h>

#include <Hedgehog/MirageCore/Camera/hhCamera.h>
#include <Hedgehog/Universe/Engine/hhStateMachine.h>
#include <Sonic/System/GameObject.h>

namespace Sonic
{
    class CCamera : public CGameObject, public Hedgehog::Universe::TStateMachine<CCamera>
    {
    public:
        class CMyCamera : public Hedgehog::Mirage::CCamera
        {
        public:
            Hedgehog::Math::CMatrix m_InputView;
        } m_MyCamera;

        INSERT_PADDING(0x228);
        float m_FieldOfView;
        INSERT_PADDING(0x54);
    };

    ASSERT_OFFSETOF(CCamera::CMyCamera, m_InputView, 0xC0);
    ASSERT_OFFSETOF(CCamera, m_MyCamera, 0x110);
    ASSERT_OFFSETOF(CCamera, m_FieldOfView, 0x438);
    ASSERT_SIZEOF(CCamera, 0x490);
}