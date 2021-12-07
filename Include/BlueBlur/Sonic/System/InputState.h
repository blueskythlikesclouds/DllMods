#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/Thread/hhHolderBase.h>
#include <Hedgehog/Base/Thread/hhSynchronizedObject.h>

#include <Sonic/System/PadState.h>

namespace Sonic
{
    class CInputState : public Hedgehog::Base::CSynchronizedObject
    {
    public:
        static constexpr CInputState* ms_pInstance = (CInputState*)0x1E76160;

        SPadState m_PadStates[40];
        INSERT_PADDING(0x50);
        uint32_t m_CurrentPadStateIndex;
        INSERT_PADDING(0x4);

        static SPadState* GetPadState()
        {
            const Hedgehog::Base::CHolderBase holder(ms_pInstance, true);
            return &ms_pInstance->m_PadStates[ms_pInstance->m_CurrentPadStateIndex];
        }
    };

    ASSERT_OFFSETOF(CInputState, m_PadStates, 0x4);
    ASSERT_OFFSETOF(CInputState, m_CurrentPadStateIndex, 0xD74);
    ASSERT_SIZEOF(CInputState, 0xD7C);
}