#pragma once

#include "Configuration.h"

class FreeCamera
{
    Configuration* config;

    Sonic::CCamera* camera;

    Eigen::Vector3f position;
    Eigen::Quaternionf rotation;

    float moveSpeed;
    float fieldOfView;
    float aspectRatio;

public:
    void update(const Sonic::SPadState& padState, float elapsedTime);
    void reset();

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    FreeCamera(Configuration* config, Sonic::CCamera* camera);
};
