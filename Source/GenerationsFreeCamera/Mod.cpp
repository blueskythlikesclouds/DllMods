#include "Configuration.h"
#include "FreeCamera.h"
#include "UpdateDirector.h"

#include <unordered_map>
#include <unordered_set>

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) { return TRUE; }

FUNCTION_PTR(void, __thiscall, procMsgSetPosition, 0xE772D0, Sonic::Player::CPlayer* This, Sonic::Message::MsgSetPosition& msg);

const char* const DISPLAY_COMMAND = (const char*)0x15C7104;
bool* const ENABLE_HUD = (bool*)0x1A430D7;
bool* const ENABLE_BLUR = (bool*)0x1A43103;
bool* const ENABLE_DOF = (bool*)0x1A4358D;
const char* const STAGE_ID = (const char*)0x1E774D4;

std::unique_ptr<Configuration> config;

std::unordered_map<Sonic::CCamera*, std::unique_ptr<FreeCamera>> cameras;

bool enableFreeCamera;
bool pauseGame;
bool handledPauseRequest;

bool wasSelect;
bool isSelect;

bool wasTeleport;
bool isTeleport;

bool wasStart;
bool isStart;

float timeParam;

HOOK(void, __fastcall, UpdateCamera, 0x10FB770, Sonic::CCamera* This, void* _, const Hedgehog::Universe::SUpdateInfo& updateInfo)
{
    if (!cameras[This] || !enableFreeCamera)
        originalUpdateCamera(This, _, updateInfo);
}

void __cdecl addCamera(Sonic::CCamera* camera)
{
    if (!cameras[camera])
        cameras[camera] = std::make_unique<FreeCamera>(config.get(), camera);
}

uint32_t addCameraTrampolineReturnAddress = 0x10FC943;
uint32_t addCameraTrampolineFunctionAddress = 0xD601F0;

void __declspec(naked) addCameraTrampoline()
{
    __asm
    {
        push edi
        call addCamera
        add esp, 4

        call[addCameraTrampolineFunctionAddress]
        jmp[addCameraTrampolineReturnAddress]
    }
}

HOOK(void*, __fastcall, CameraDestructor, 0x10FB630, Sonic::CCamera* This)
{
    cameras.erase(This);
    return originalCameraDestructor(This);
}

uint32_t stageStateUpdatingMidAsmHookReturnAddress = 0xD06D9A;
uint32_t stageStateUpdatingMidAsmHookJumpAddress = 0xD06E26;

void __declspec(naked) stageStateUpdatingMidAsmHook()
{
    __asm
    {
        cmp enableFreeCamera, 1
        jz jump

        mov eax, [esp + 0x14]
        test dword ptr[eax + 8], 0x400
        jmp[stageStateUpdatingMidAsmHookReturnAddress]

        jump:
        jmp[stageStateUpdatingMidAsmHookJumpAddress]
    }
}

uint32_t eventManagerMidAsmHookReturnAddress = 0xB27FAC;
uint32_t eventManagerMidAsmHookJumpAddress = 0xB27FB9;

void __declspec(naked) eventManagerMidAsmHook()
{
    __asm
    {
        cmp enableFreeCamera, 1
        jz jump

        test dword ptr[esi + 8], 0x400
        jmp[eventManagerMidAsmHookReturnAddress]

        jump:
        jmp[eventManagerMidAsmHookJumpAddress]
    }
}

void* updateDirectorCustom(void* context, float elapsedTime)
{
    const bool loadObjectsInRange = config->loadObjectsInRange && strcmp(STAGE_ID, "blb") != 0;

    addUpdateCommand(context, elapsedTime, "2");
    addUpdateCommand(context, elapsedTime, "3");
    addUpdateCommand(context, elapsedTime, "b");

    if (loadObjectsInRange)
        addUpdateCommand(context, elapsedTime, DISPLAY_COMMAND);

    callUnknownFunction(context, elapsedTime);

    addRenderCommand(context, elapsedTime, "2");
    addRenderCommand(context, elapsedTime, "3");
    addRenderCommand(context, elapsedTime, "b");

    if (loadObjectsInRange)
        addRenderCommand(context, elapsedTime, DISPLAY_COMMAND);

    finishRenderer(context);
    return finishUpdater(context);
}

HOOK(void*, __fastcall, UpdateDirectorSimple, 0x1105A60, void* This, void* Edx, void* context, float elapsedTime)
{
    if (pauseGame)
        return updateDirectorCustom(context, elapsedTime);

    return originalUpdateDirectorSimple(This, Edx, context, elapsedTime);
}

HOOK(void*, __fastcall, UpdateDirectorNormal, 0x11B60B0, void* This, void* Edx, void* context, float elapsedTime)
{
    if (pauseGame)
        return updateDirectorCustom(context, elapsedTime);

    return originalUpdateDirectorNormal(This, Edx, context, elapsedTime);
}

HOOK(void*, __fastcall, UpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
{
    if (cameras.empty())
        return originalUpdateApplication(This, Edx, elapsedTime, a3);

    timeParam = elapsedTime;

    const Sonic::SPadState& padState = Sonic::CInputState::GetInstance()->GetPadState();

    isSelect = padState.IsDown(Sonic::eKeyState_Select);

    if (isSelect && padState.IsDown(Sonic::eKeyState_Start) && !handledPauseRequest)
    {
        pauseGame = enableFreeCamera && !pauseGame;
        handledPauseRequest = true;
    }
    else if (wasSelect && !isSelect)
    {
        if (enableFreeCamera && !handledPauseRequest)
        {
            enableFreeCamera = false;
            pauseGame = false;
        }
        else
        {
            if (!handledPauseRequest)
                pauseGame = true;

            enableFreeCamera = true;

            for (auto& it : cameras)
                it.second->reset();
        }

        handledPauseRequest = false;
    }

    wasSelect = isSelect;

    if (!enableFreeCamera)
        return originalUpdateApplication(This, Edx, elapsedTime, a3);

    for (const auto& [_, freeCamera] : cameras)
        freeCamera->update(padState, elapsedTime);

    isTeleport = config->teleportPlayer.isDown(padState);

    const bool previousPauseGame = pauseGame;
    const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();

    if (!wasTeleport && isTeleport && playerContext)
    {
        for (auto& [camera, _] : cameras)
        {
            if (!*reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(camera) + 0x468))
                continue;

            Sonic::Message::MsgSetPosition msg(camera->m_MyCamera.m_Position + camera->m_MyCamera.m_Direction * 2 * ((float)M_PI / 4.0f / camera->m_FieldOfView));
            msg.m_Position.y() -= 0.5f;

            procMsgSetPosition(playerContext->m_pPlayer, msg);

            playerContext->SetVelocity(Hedgehog::Math::CVector::Zero());

            pauseGame = false;
            break;
        }
    }

    wasTeleport = isTeleport;

    const bool previousEnableHud = *ENABLE_HUD;
    const bool previousEnableBlur = *ENABLE_BLUR;
    const bool previousEnableDof = *ENABLE_DOF;

    *ENABLE_HUD &= !config->disableHud;
    *ENABLE_BLUR &= !config->disableBlur;
    *ENABLE_DOF &= !config->disableDepthOfField;

    isStart = padState.IsDown(Sonic::eKeyState_Start);
    pauseGame &= config->frameAdvance ? wasStart || !isStart : !isStart;
    wasStart = isStart;

    void* result = originalUpdateApplication(This, Edx, elapsedTime, a3);

    *ENABLE_HUD = previousEnableHud;
    *ENABLE_BLUR = previousEnableBlur;
    *ENABLE_DOF = previousEnableDof;

    pauseGame = previousPauseGame;

    return result;
}


// Make eye adaptation work when game is paused

void setTimeParam(DX_PATCH::IDirect3DDevice9* device)
{
    if (!pauseGame) 
        return;

    float param[] = { timeParam, 0, 0, 0 };
    device->SetPixelShaderConstantF(68, param, 1);
}

// MTFx
FUNCTION_PTR(void, __cdecl, SceneTraversed_ScreenDefaultExec, 0x6517E0, void*, void*);

void sceneTraverseTonemapFilter(void* A1, void* A2)
{
    hh::mr::CRenderingDevice* renderingDevice = **(hh::mr::CRenderingDevice***)((char*)A1 + 16);
    setTimeParam(renderingDevice->m_pD3DDevice);
    SceneTraversed_ScreenDefaultExec(A1, A2);
}

// FxPipeline
HOOK(void, __stdcall, CalculateAdaptedLuminance, 0x10C4830, Sonic::CFxJob* This)
{
    setTimeParam(This->m_pScheduler->m_pMisc->m_pDevice->m_pD3DDevice);
    originalCalculateAdaptedLuminance(This);
}

extern "C" void __declspec(dllexport) Init()
{
    INIReader reader("GenerationsFreeCamera.ini");
    if (reader.ParseError() != 0)
        MessageBox(NULL, L"Failed to parse GenerationsFreeCamera.ini", NULL, MB_ICONERROR);

    config = std::make_unique<Configuration>(Configuration(reader));

    INSTALL_HOOK(UpdateCamera);

    WRITE_JUMP(0x10FC93E, addCameraTrampoline);
    INSTALL_HOOK(CameraDestructor);

    WRITE_JUMP(0xD06D8F, stageStateUpdatingMidAsmHook);
    WRITE_JUMP(0xB27FA5, eventManagerMidAsmHook);

    INSTALL_HOOK(UpdateDirectorSimple);
    INSTALL_HOOK(UpdateDirectorNormal);

    INSTALL_HOOK(UpdateApplication);

    // Patch update director commands
    if (config->loadObjectsInRange)
    {
        // Activation Manager
        // "6" -> "4"
        WRITE_MEMORY(0xEA5A90, char*, DISPLAY_COMMAND);
        WRITE_MEMORY(0xEA6E24, char*, DISPLAY_COMMAND);
    }

    // "6" -> "3"
    WRITE_MEMORY(0xCD46DC, uint8_t, 0x00); // Object Instancer
    WRITE_MEMORY(0xCD73A0, uint8_t, 0x00); // Instancer Manager

    WRITE_MEMORY(0x13DF98C, void*, &sceneTraverseTonemapFilter);
    INSTALL_HOOK(CalculateAdaptedLuminance);
}