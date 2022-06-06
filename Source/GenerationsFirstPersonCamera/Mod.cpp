#include "../GenerationsParameterEditor/Include/DebugDrawText.h"

hh::math::CQuaternion prevRotation;

void UpdateCamera()
{
    const auto pPlayerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
    if (!pPlayerContext)
        return;

    const auto pPlayer = pPlayerContext->m_pPlayer;

    const auto spCamera = pPlayer->m_pMember->m_pGameDocument->GetWorld()->GetCamera();
    spCamera->m_MyCamera.m_Near = 0.03f;
    spCamera->m_FieldOfView = (float)M_PI_2;
    spCamera->m_MyCamera.m_Projection = Eigen::CreatePerspectiveMatrix(spCamera->m_FieldOfView, spCamera->m_MyCamera.m_AspectRatio, spCamera->m_MyCamera.m_Near, spCamera->m_MyCamera.m_Far);

    const bool useHead = pPlayerContext->GetVelocity().squaredNorm() < 0.01f || true;

    const auto spRotNode = pPlayer->m_spCharacterModel->GetNode(useHead ? "Head" : "Reference");
    const auto spPosNode = pPlayer->m_spCharacterModel->GetNode("Nose");

    const hh::math::CMatrix& rRotTransform = spRotNode->GetWorldMatrix();
    const hh::math::CMatrix& rPosTransform = spPosNode->GetWorldMatrix();

    hh::math::CQuaternion rotation(rRotTransform.rotation() * Eigen::AngleAxisf((float)(useHead ? -M_PI_2 : M_PI), useHead ? Eigen::Vector3f::UnitZ() : Eigen::Vector3f::UnitY()));
    hh::math::CVector position(rPosTransform.translation());
    
    position += (rotation * hh::math::CVector::UnitY()).normalized() * 0.04f;
    rotation = prevRotation.slerp(0.04f, rotation);

    spCamera->m_MyCamera.m_Position = position;
    spCamera->m_MyCamera.m_Direction = (rotation * hh::math::CVector::UnitZ()).normalized();

    spCamera->m_MyCamera.m_View = (Eigen::Translation3f(position) * rotation).inverse();

    spCamera->m_MyCamera.m_InputView = spCamera->m_MyCamera.m_View;

    prevRotation = rotation;
}

void __declspec(naked) _11B68DAMidAsmHook()
{
    static uint32_t funcAddr = 0xD67820;
    static uint32_t retAddr = 0x11B68E0;
    __asm
    {
        call UpdateCamera
        call [funcAddr]
        mov ecx, [esi + 4]
        mov ecx, [ecx + 38h]
        jmp[retAddr]
    }
}

void __declspec(naked) CCameraMidAsmHook()
{
    __asm
    {
        call UpdateCamera
        pop edi
        pop esi
        pop ebx
        mov esp, ebp
        pop ebp
        retn 4
    }
}

extern "C" void __declspec(dllexport) Init()
{
    // Prevent CCamera::UpdateParallel
    // WRITE_MEMORY(0x10FB770, uint8_t, 0xC2, 0x04, 0x00);

    // Prevent CCamera::UpdateSerial
    // WRITE_MEMORY(0x10F8E00, uint8_t, 0xC2, 0x04, 0x00);

    // Delay renderer update so the models can sync with the camera properly
    WRITE_NOP(0x11B63EC, 5);
    WRITE_JUMP(0x11B68DA, _11B68DAMidAsmHook);

    WRITE_JUMP(0x10FC91B, CCameraMidAsmHook);

    WRITE_MEMORY(0x127333B, char*, "sn_jump_d_loop");
    WRITE_MEMORY(0x1274667, char*, "sn_jump_d_loop");
    WRITE_MEMORY(0x12732D4, char*, "sn_jump_d_loop");
    WRITE_MEMORY(0x127A06D, char*, "sn_jump_d_loop");
}