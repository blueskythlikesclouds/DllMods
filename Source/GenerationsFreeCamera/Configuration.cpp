#include "Configuration.h"

#define GET_FLOAT(section, name, defaultValue) \
    reader->GetFloat(section, name, defaultValue)

#define GET_BOOL(section, name, defaultValue) \
    reader->GetBoolean(section, name, defaultValue)

#define GET_KEY(section, name, defaultValue) \
    reader, section, name, defaultValue

Configuration::Configuration(INIReader* reader) :
    slowDown(GET_KEY("Speed", "SlowDown", LEFT_TRIGGER)),
    speedUp(GET_KEY("Speed", "SpeedUp", RIGHT_TRIGGER)),

    descend(GET_KEY("Position", "Descend", LEFT_BUMPER)),
    ascend(GET_KEY("Position", "Ascend", RIGHT_BUMPER)),
    descendAscendSpeed(GET_FLOAT("Position", "DescendAscendSpeed", 0.5f)),

    resetMoveSpeed(GET_KEY("Position", "ResetMoveSpeed", A)),
    decreaseMoveSpeed(GET_KEY("Position", "DecreaseMoveSpeed", B)),
    increaseMoveSpeed(GET_KEY("Position", "IncreaseMoveSpeed", X)),

    moveSpeed(GET_FLOAT("Position", "MoveSpeed", 0.5f)),
    moveSpeedDecreaseRatio(GET_FLOAT("Position", "MoveSpeedDecreaseRatio", -0.01f)),
    moveSpeedIncreaseRatio(GET_FLOAT("Position", "MoveSpeedIncreaseRatio", +0.01f)),

    moveSpeedSlowDownMultiplier(GET_FLOAT("Position", "MoveSpeedSlowDownMultiplier", 0.25)),
    moveSpeedSpeedUpMultiplier(GET_FLOAT("Position", "MoveSpeedSpeedUpMultiplier", 4.0)),

    resetRotation(GET_KEY("Rotation", "ResetRotation", RIGHT_STICK)),
    rotateLeft(GET_KEY("Rotation", "RotateLeft", DPAD_LEFT)),
    rotateRight(GET_KEY("Rotation", "RotateRight", DPAD_RIGHT)),

    rotateSpeed(GET_FLOAT("Rotation", "RotateSpeed", 1.5f)),

    rotateSpeedSlowDownMultiplier(GET_FLOAT("Rotation", "RotateSpeedSlowDownMultiplier", 1.0f)),
    rotateSpeedSpeedUpMultiplier(GET_FLOAT("Rotation", "RotateSpeedSpeedUpMultiplier", 1.0f)),

    useFixedYawAxis(GET_BOOL("Rotation", "UseFixedYawAxis", true)),

    resetFieldOfView(GET_KEY("FieldOfView", "ResetFieldOfView", Y)),
    decreaseFieldOfView(GET_KEY("FieldOfView", "DecreaseFieldOfView", DPAD_DOWN)),
    increaseFieldOfView(GET_KEY("FieldOfView", "IncreaseFieldOfView", DPAD_UP)),

    defaultFieldOfView(GET_FLOAT("FieldOfView", "DefaultFieldOfView", 45.0f)),

    fieldOfViewDecreaseRatio(GET_FLOAT("FieldOfView", "FieldOfViewDecreaseRatio", -1.0f)),
    fieldOfViewIncreaseRatio(GET_FLOAT("FieldOfView", "FieldOfViewIncreaseRatio", +1.0f)),

    teleportPlayer(GET_KEY("Misc", "TeleportPlayer", LEFT_STICK)),

    disableBlur(GET_BOOL("Misc", "DisableBlur", true)),
    disableDepthOfField(GET_BOOL("Misc", "DisableDepthOfField", false)),
    disableHud(GET_BOOL("Misc", "DisableHud", true)),
    loadObjectsInRange(GET_BOOL("Misc", "LoadObjectsInRange", true))
{
}
