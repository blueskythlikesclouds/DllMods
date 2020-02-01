#pragma once

#include "KeyBinding.h"

class Configuration
{
public:
    const KeyBinding slowDown;
    const KeyBinding speedUp;

    const KeyBinding descend;
    const KeyBinding ascend;
    const float descendAscendSpeed;

    const KeyBinding resetMoveSpeed;
    const KeyBinding decreaseMoveSpeed;
    const KeyBinding increaseMoveSpeed;

    const float moveSpeed;
    const float moveSpeedDecreaseRatio;
    const float moveSpeedIncreaseRatio;

    const float moveSpeedSlowDownMultiplier;
    const float moveSpeedSpeedUpMultiplier;

    const KeyBinding resetRotation;
    const KeyBinding rotateLeft;
    const KeyBinding rotateRight;

    const float rotateSpeed;

    const float rotateSpeedSlowDownMultiplier;
    const float rotateSpeedSpeedUpMultiplier;

    const bool useFixedYawAxis;

    const KeyBinding resetFieldOfView;
    const KeyBinding decreaseFieldOfView;
    const KeyBinding increaseFieldOfView;

    const float defaultFieldOfView;

    const float fieldOfViewDecreaseRatio;
    const float fieldOfViewIncreaseRatio;

    const KeyBinding teleportPlayer;

    const bool disableBlur;
    const bool disableDepthOfField;
    const bool disableHud;
    const bool loadObjectsInRange;

    Configuration(INIReader* reader);
};
