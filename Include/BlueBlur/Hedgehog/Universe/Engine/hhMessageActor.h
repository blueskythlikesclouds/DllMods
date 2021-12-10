#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/Type/hhSharedString.h>
#include <Hedgehog/Universe/Engine/hhMessageProcess.h>
#include <Hedgehog/Universe/Engine/hhParallelJob.h>

namespace Hedgehog::Universe
{
    class Message;
    class CMessageActor;
    class IStateMachineMessageReceiver;

    static inline FUNCTION_PTR(CMessageActor*, __thiscall, fpCMessageActorCtor, 0x768A00, CMessageActor* This);

    static inline FUNCTION_PTR(void, __thiscall, fpCMessageActorExecuteParallelJob, 0x7680C0, CMessageActor* This, const SUpdateInfo& updateInfo);

    static inline FUNCTION_PTR(void, __thiscall, fpCMessageActorSendMessageByID, 0x768340,
        CMessageActor* This, const char* path, size_t line, size_t actorID, const boost::shared_ptr<Message>& spMessage, float time);   
    
    static inline FUNCTION_PTR(void, __thiscall, fpCMessageActorSendMessageByCategory, 0x7684E0,
        CMessageActor* This, const char* path, size_t line, const Hedgehog::Base::CSharedString& actorCategory, const boost::shared_ptr<Message>& spMessage, float time);

    static inline FUNCTION_PTR(void, __thiscall, fpCMessageActorSendMessageImmByID, 0x768810,
        CMessageActor* This, const char* path, size_t line, size_t actorID, const boost::shared_ptr<Message>& spMessage);

    static inline FUNCTION_PTR(void, __thiscall, fpCMessageActorSendMessageImmByCategory, 0x768770,
        CMessageActor* This, const char* path, size_t line, const Hedgehog::Base::CSharedString& actorCategory, const boost::shared_ptr<Message>& spMessage);

    class CMessageActor : public IMessageProcess, public Base::CObject, public IParallelJob
    {
    public:
        INSERT_PADDING(0x24);
        uint32_t m_ActorID;
        INSERT_PADDING(0x4C);

        CMessageActor(const null_ctor&) : IMessageProcess(null_ctor{}), CObject(null_ctor{}), IParallelJob(null_ctor{}) {}

        CMessageActor() : CMessageActor(null_ctor{})
        {
            fpCMessageActorCtor(this);
        }

        virtual void ExecuteParallelJob(const SUpdateInfo& updateInfo) override
        {
            fpCMessageActorExecuteParallelJob(this, updateInfo);
        }

        virtual bool ProcessMessage(const Message& message, bool flag)
        {
            return false;
        }

        virtual IStateMachineMessageReceiver* GetStateMachineMessageReceiver(bool flag)
        {
            return nullptr;
        }

#undef SendMessage

        void SendMessage(const size_t actorID, const boost::shared_ptr<Message>& spMessage, float time = 0.0f)
        {
            fpCMessageActorSendMessageByID(this, nullptr, 0, actorID, spMessage, time);
        }      
        
        void SendMessage(const Hedgehog::Base::CSharedString& actorCategory, const boost::shared_ptr<Message>& spMessage, float time = 0.0f)
        {
            fpCMessageActorSendMessageByCategory(this, nullptr, 0, actorCategory, spMessage, time);
        }      
        
        void SendMessageImm(const size_t actorID, const boost::shared_ptr<Message>& spMessage)
        {
            fpCMessageActorSendMessageImmByID(this, nullptr, 0, actorID, spMessage);
        }      
        
        void SendMessageImm(const Hedgehog::Base::CSharedString& actorCategory, const boost::shared_ptr<Message>& spMessage)
        {
            fpCMessageActorSendMessageImmByCategory(this, nullptr, 0, actorCategory, spMessage);
        }
    };

    //ASSERT_OFFSETOF(CMessageActor, m_Category, 0x14);
    ASSERT_OFFSETOF(CMessageActor, m_ActorID, 0x2C);
    ASSERT_SIZEOF(CMessageActor, 0x7C);
}

