#include "Configuration.h"

#define GET_FLOAT(section, name, defaultValue) \
    reader.GetFloat(section, name, defaultValue)

#define GET_BOOL(section, name, defaultValue) \
    reader.GetBoolean(section, name, defaultValue)

#define GET_KEY(section, name, defaultValue) \
    reader, section, name, defaultValue

Configuration::Configuration(const INIReader& reader) :
    slowDown(GET_KEY("Speed", "SlowDown", Sonic::eKeyState_LeftTrigger)),
    speedUp(GET_KEY("Speed", "SpeedUp", Sonic::eKeyState_RightTrigger)),

    descend(GET_KEY("Position", "Descend", Sonic::eKeyState_LeftBumper)),
    ascend(GET_KEY("Position", "Ascend", Sonic::eKeyState_RightBumper)),
    descendAscendSpeed(GET_FLOAT("Position", "DescendAscendSpeed", 0.5f)),

    resetMoveSpeed(GET_KEY("Position", "ResetMoveSpeed", Sonic::eKeyState_A)),
    decreaseMoveSpeed(GET_KEY("Position", "DecreaseMoveSpeed", Sonic::eKeyState_B)),
    increaseMoveSpeed(GET_KEY("Position", "IncreaseMoveSpeed", Sonic::eKeyState_X)),

    moveSpeed(GET_FLOAT("Position", "MoveSpeed", 0.5f)),
    moveSpeedDecreaseRatio(GET_FLOAT("Position", "MoveSpeedDecreaseRatio", -0.01f)),
    moveSpeedIncreaseRatio(GET_FLOAT("Position", "MoveSpeedIncreaseRatio", +0.01f)),

    moveSpeedSlowDownMultiplier(GET_FLOAT("Position", "MoveSpeedSlowDownMultiplier", 0.25)),
    moveSpeedSpeedUpMultiplier(GET_FLOAT("Position", "MoveSpeedSpeedUpMultiplier", 4.0)),

    resetRotation(GET_KEY("Rotation", "ResetRotation", Sonic::eKeyState_RightStick)),
    rotateLeft(GET_KEY("Rotation", "RotateLeft", Sonic::eKeyState_DpadLeft)),
    rotateRight(GET_KEY("Rotation", "RotateRight", Sonic::eKeyState_DpadRight)),

    rotateSpeed(GET_FLOAT("Rotation", "RotateSpeed", 1.5f)),

    rotateSpeedSlowDownMultiplier(GET_FLOAT("Rotation", "RotateSpeedSlowDownMultiplier", 1.0f)),
    rotateSpeedSpeedUpMultiplier(GET_FLOAT("Rotation", "RotateSpeedSpeedUpMultiplier", 1.0f)),

    useFixedYawAxis(GET_BOOL("Rotation", "UseFixedYawAxis", true)),

    resetFieldOfView(GET_KEY("FieldOfView", "ResetFieldOfView", Sonic::eKeyState_Y)),
    decreaseFieldOfView(GET_KEY("FieldOfView", "DecreaseFieldOfView", Sonic::eKeyState_DpadDown)),
    increaseFieldOfView(GET_KEY("FieldOfView", "IncreaseFieldOfView", Sonic::eKeyState_DpadUp)),

    defaultFieldOfView(GET_FLOAT("FieldOfView", "DefaultFieldOfView", 45.0f)),

    fieldOfViewDecreaseRatio(GET_FLOAT("FieldOfView", "FieldOfViewDecreaseRatio", -1.0f)),
    fieldOfViewIncreaseRatio(GET_FLOAT("FieldOfView", "FieldOfViewIncreaseRatio", +1.0f)),

    teleportPlayer(GET_KEY("Misc", "TeleportPlayer", Sonic::eKeyState_LeftStick)),

    disableBlur(GET_BOOL("Misc", "DisableBlur", true)),
    disableDepthOfField(GET_BOOL("Misc", "DisableDepthOfField", false)),
    disableHud(GET_BOOL("Misc", "DisableHud", true)),
    loadObjectsInRange(GET_BOOL("Misc", "LoadObjectsInRange", true)),
    frameAdvance(GET_BOOL("Misc", "FrameAdvance", false))
{
}
