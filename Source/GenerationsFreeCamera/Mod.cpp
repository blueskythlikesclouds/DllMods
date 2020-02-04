#include "Camera.h"
#include "Configuration.h"
#include "FreeCamera.h"
#include "InputState.h"
#include "UpdateDirector.h"

#include <unordered_map>
#include <unordered_set>

FUNCTION_PTR(void*, __thiscall, playerProcessMsgSetPosition, 0xE772D0, void* This, void* message);

const char* const DISPLAY_COMMAND = (const char*)0x15C7104;
bool* const ENABLE_HUD = (bool*)0x1A430D7;
bool* const ENABLE_BLUR = (bool*)0x1A43103;
bool* const ENABLE_DOF = (bool*)0x1A4358D;
void** const PLAYER_CONTEXT = (void**)0x1E5E2F0;
const char* const STAGE_ID = (const char*)0x1E774D4;

Configuration* config;

std::unordered_map<Camera*, FreeCamera*> cameras;

bool enableFreeCamera;
bool pauseGame;
bool handledPauseRequest;

HOOK(void, __fastcall, UpdateCamera, 0x10FB770, Camera* This, void* Edx, uint32_t a2)
{
    if (!cameras[This] || !enableFreeCamera)
        originalUpdateCamera(This, Edx, a2);
}

void __cdecl addCamera(Camera* camera)
{
    if (!cameras[camera])
        cameras[camera] = new FreeCamera(config, camera);
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

HOOK(void*, __fastcall, CameraDestructor, 0x10FB630, Camera* This, void* Edx)
{
    auto it = cameras.find(This);
    if (it != cameras.end())
    {
        delete it->second;
        cameras.erase(it);
    }

    return originalCameraDestructor(This, Edx);
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

    InputState* inputState = getInputState();

    if (inputState->isDown(SELECT) && inputState->isDown(START) && !handledPauseRequest)
    {
        pauseGame = enableFreeCamera && !pauseGame;
        handledPauseRequest = true;
    }
    else if (inputState->isReleased(SELECT))
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

    if (!enableFreeCamera)
        return originalUpdateApplication(This, Edx, elapsedTime, a3);

    for (auto& it : cameras)
        it.second->update(elapsedTime);

    if (config->teleportPlayer.isTapped(inputState) && *PLAYER_CONTEXT)
    {
        void* player = *(void**)((uint32_t)*PLAYER_CONTEXT + 0x110);

        struct MsgSetPosition
        {
            uint8_t gap0[16];
            Eigen::Vector3f value;
            uint8_t gap1C[4];
        };

        for (auto& it : cameras)
        {
            if (!it.first->field468)
                continue;

            alignas(16) MsgSetPosition msgSetPosition;
            msgSetPosition.value = it.first->position + it.first->direction * 2 * ((float)M_PI / 4.0f / it.first->fieldOfView);
            msgSetPosition.value.y() -= 0.5f;
            playerProcessMsgSetPosition(player, &msgSetPosition);

            // Reset velocity
            uint32_t result = *(uint32_t*)((uint32_t)player + 172);
            *(Eigen::Vector3f*)(result + 656) = Eigen::Vector3f(0, 0, 0);
            *(bool*)(result + 1512) = true;
            *(bool*)(result + 1513) = false;

            break;
        }
    }

    const bool previousEnableHud = *ENABLE_HUD;
    const bool previousEnableBlur = *ENABLE_BLUR;
    const bool previousEnableDof = *ENABLE_DOF;

    const bool previousPauseGame = pauseGame;

    *ENABLE_HUD &= !config->disableHud;
    *ENABLE_BLUR &= !config->disableBlur;
    *ENABLE_DOF &= !config->disableDepthOfField;

    pauseGame &= !inputState->isDown(START);
    pauseGame &= !config->teleportPlayer.isTapped(inputState);

    void* result = originalUpdateApplication(This, Edx, elapsedTime, a3);

    *ENABLE_HUD = previousEnableHud;
    *ENABLE_BLUR = previousEnableBlur;
    *ENABLE_DOF = previousEnableDof;

    pauseGame = previousPauseGame;

    return result;
}

extern "C" void __declspec(dllexport) Init()
{
    INIReader reader("GenerationsFreeCamera.ini");
    if (reader.ParseError() != 0)
        MessageBox(NULL, L"Failed to parse GenerationsFreeCamera.ini", NULL, MB_ICONERROR);

    config = new Configuration(&reader);

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
}