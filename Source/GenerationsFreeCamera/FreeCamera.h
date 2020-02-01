#pragma once

#include "Camera.h"
#include "Configuration.h"

class FreeCamera
{
    Configuration* config;

    Camera* camera;

    Eigen::Vector3f position;
    Eigen::Quaternionf rotation;

    float moveSpeed;
    float fieldOfView;
    float aspectRatio;

public:
    void update(float elapsedTime);
    void reset();

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    FreeCamera(Configuration* config, Camera* camera);
};
