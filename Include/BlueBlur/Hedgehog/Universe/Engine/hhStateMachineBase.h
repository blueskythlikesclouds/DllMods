#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/hhObject.h>
#include <Hedgehog/Universe/Engine/hhMessageProcess.h>
#include <Hedgehog/Universe/Engine/hhStateMachineMessageReceiver.h>
#include <Hedgehog/Universe/Engine/hhUpdateInfo.h>

namespace Hedgehog::Universe
{
    class Message;
    class CStateMachineBase;

    static FUNCTION_PTR(bool, __thiscall, fpCStateMachineBaseReceiveMessage, 0x76B640,
        CStateMachineBase* This, const Message& message, bool flag);

    static FUNCTION_PTR(void, __thiscall, fpCStateMachineBaseUpdate, 0x76DF00,
        CStateMachineBase* This, const SUpdateInfo& updateInfo);

    class CStateMachineBase : public IStateMachineMessageReceiver, public Base::CObject
    {
    public:
        class CStateBase : public IMessageProcess, public Base::CObject
        {
        public:
            void* m_pOwner;
            CStateMachineBase* m_pStateMachine;
            float m_TotalTime;
            INSERT_PADDING(0x4);
            Base::CSharedString m_Name;
            INSERT_PADDING(0x44);

            virtual bool ProcessMessage(const Message& message, bool flag)
            {
                return false;
            }

            virtual ~CStateBase() = default;

            virtual void Enter() {}
            virtual void Update() {}

            virtual void _CStateBase10() {}

            virtual void Leave() {}

            virtual void _CStateBase18() {}
            virtual void _CStateBase1C() {}

            virtual const Base::CSharedString& GetName() 
            {
                return m_Name;
            }
        };

        INSERT_PADDING(0x18);
        void* m_pOwner;
        float m_TotalTime;
        SUpdateInfo m_UpdateInfo;
        INSERT_PADDING(0x34);

        virtual bool ReceiveMessage(const Message& message, bool flag) override
        {
            return fpCStateMachineBaseReceiveMessage(this, message, flag);
        }

        virtual ~CStateMachineBase() = default;

        void Update(const SUpdateInfo& updateInfo)
        {
            fpCStateMachineBaseUpdate(this, updateInfo);
        }
    };

    ASSERT_OFFSETOF(CStateMachineBase::CStateBase, m_pOwner, 0x8);
    ASSERT_OFFSETOF(CStateMachineBase::CStateBase, m_pStateMachine, 0xC);
    ASSERT_OFFSETOF(CStateMachineBase::CStateBase, m_TotalTime, 0x10);
    ASSERT_OFFSETOF(CStateMachineBase::CStateBase, m_Name, 0x18);
    ASSERT_SIZEOF(CStateMachineBase::CStateBase, 0x60);
    ASSERT_OFFSETOF(CStateMachineBase, m_pOwner, 0x1C);
    ASSERT_OFFSETOF(CStateMachineBase, m_TotalTime, 0x20);
    ASSERT_OFFSETOF(CStateMachineBase, m_UpdateInfo, 0x24);
    ASSERT_SIZEOF(CStateMachineBase, 0x64);
}