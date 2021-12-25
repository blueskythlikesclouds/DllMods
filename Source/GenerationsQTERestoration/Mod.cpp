#include "ArchiveTreePatcher.h"

#if !_DEBUG
void __cdecl boost::throw_exception(class stdext::exception const&) {}
#endif

uint32_t pE5CD90 = 0xE5CD90;

void fE5CD90(
    uint32_t targetActorID, 
    Sonic::Player::CPlayerContext* pPlayerContext,
    const hh::math::CVector* pPosition, 
    Sonic::Player::CPlayerSpeedStateReactionLand::EAnimationType animationType, 
    float minVelocity,
    float maxVelocity,
    int score, 
    int difficulty)
{
    __asm
    {
        push difficulty
        push score
        push maxVelocity
        push minVelocity
        push animationType
        push pPosition
        push pPlayerContext
        mov eax, targetActorID
        call[pE5CD90]
    }
}

void ProcMsgHitReactionPlate(Sonic::Player::CPlayerSpeed* This, const Sonic::Message::MsgHitReactionPlate& message)
{
    const float minVelocity = message.m_JumpMinVelocity <= 0 ? 40 : message.m_JumpMinVelocity;
    const float maxVelocity = message.m_JumpMaxVelocity <= 0 ? 60 : message.m_JumpMaxVelocity;

    if (message.m_Type)
    {
        if (message.m_Type != 5) // End
        {
            const auto pState = This->GetContext()->SetState<Sonic::Player::CPlayerSpeedStateReactionLand>();
            pState->m_TargetActorID = message.m_TargetActorID;
            pState->m_JumpMinVelocity = minVelocity;
            pState->m_JumpMaxVelocity = maxVelocity;

            This->GetContext()->m_spMatrixNodeTransform->m_Transform.SetPosition(message.m_Position + message.m_Direction * 0.25f);

            const auto yawDirection = hh::math::CVector(message.m_Direction.x(), 0, message.m_Direction.z()).normalized();
            const float yaw = atan2(yawDirection.x(), yawDirection.z());
            const float pitch = asin(-message.m_Direction.y());

            This->GetContext()->m_VerticalRotation = Eigen::AngleAxisf(pitch, Eigen::Vector3f::UnitX()) * Eigen::AngleAxisf(yaw, Eigen::Vector3f::UnitY());
            This->GetContext()->m_HorizontalRotation.setIdentity();

            hh::math::CVector targetPosition;
            This->SendMessageImm(message.m_TargetActorID, boost::make_shared<Sonic::Message::MsgGetPosition>(targetPosition));

            const auto rightDirection = message.m_Direction.cross(hh::math::CVector::UnitY()).normalized();
            const auto targetDirection = (targetPosition - message.m_Position).normalized();
            const bool facesLeft = rightDirection.dot(targetDirection) < 0;

            This->GetContext()->SetAnimation(facesLeft ? "ReactionLandL" : "ReactionLandR");
            pState->m_AnimationType = facesLeft ?
                Sonic::Player::CPlayerSpeedStateReactionLand::eAnimationType_ReactionJumpL :
                Sonic::Player::CPlayerSpeedStateReactionLand::eAnimationType_ReactionJumpR;

            This->GetContext()->PlaySound(4002046, false);
        }
    }
    else // Begin
    {
        fE5CD90(
            message.m_TargetActorID,
            This->m_spContext.get(),
            &message.m_Position,
            Sonic::Player::CPlayerSpeedStateReactionLand::eAnimationType_JumpSpring,
            minVelocity,
            maxVelocity,
            1000,
            1);
    }
}

HOOK(bool, __fastcall, CPlayerSpeedProcessMessage, 0xE6E000, hh::fnd::CMessageActor* This, void* Edx, hh::fnd::Message& message, bool flag)
{
    if (flag && message.Is<Sonic::Message::MsgHitReactionPlate>())
    {
        ProcMsgHitReactionPlate(static_cast<Sonic::Player::CPlayerSpeed*>(This),
                                static_cast<Sonic::Message::MsgHitReactionPlate&>(message));
        return true;
    }

    return originalCPlayerSpeedProcessMessage(This, Edx, message, flag);
}

void reactionJumpPlaySfx()
{
    Sonic::Player::CPlayerSpeedContext::GetInstance()->PlaySound(4002047, false);
}

void reactionJumpSetAnim()
{
    Sonic::Player::CPlayerSpeedContext::GetInstance()->SetAnimation("ReactionJumpU");
}

void __declspec(naked) reactionJumpPlaySfxTrampoline()
{
    __asm
    {
        call reactionJumpPlaySfx
        pop edi
        pop esi
        pop ebx
        mov esp, ebp
        pop ebp
        retn 0x1C
    }
}

void __declspec(naked) reactionJumpSetAnimTrampoline()
{
    __asm
    {
        call reactionJumpSetAnim
        pop edi
        pop esi
        pop ebx
        mov esp, ebp
        pop ebp
        retn
    }
}

extern "C" void __declspec(dllexport) Init()
{
    WRITE_MEMORY(0x1A47288, char*,
        "cmn_obj_kt3_springblueM_000",
        "cmn_obj_km_BreactionpanelM_000",
        "cmn_obj_km_AreactionpanelM_000",
        "cmn_obj_km_XreactionpanelM_000",
        "cmn_obj_km_YreactionpanelM_000",
        "RP_ButtonE",
        "sea_obj_km_reactionpanelQ_000");

    ArchiveTreePatcher::applyPatches();

    WRITE_MEMORY(0xEBA0DB, char*, "ReactionPlate");

    INSTALL_HOOK(CPlayerSpeedProcessMessage);

    WRITE_JUMP(0xE5D03E, reactionJumpPlaySfxTrampoline);
    WRITE_JUMP(0xE5CDB3, reactionJumpPlaySfxTrampoline);
    WRITE_JUMP(0x124B915, reactionJumpSetAnimTrampoline);

    WRITE_MEMORY(0x1274C03 + 6, char*, "sn_airboost_loop");
    WRITE_MEMORY(0x1274C6A + 6, char*, "sn_airboost_loop");
}