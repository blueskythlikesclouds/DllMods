#include "FreeCamera.h"
#include "InputState.h"

const uint32_t* const WIDTH = (uint32_t*)0x1DFDDDC;
const uint32_t* const HEIGHT = (uint32_t*)0x1DFDDE0;

void FreeCamera::update(const float elapsedTime)
{
    const float factor = elapsedTime / (1.0f / 60.0f);

    InputState* inputState = getInputState();

    const bool slowDown = config->slowDown.isDown(inputState);
    const bool speedUp = config->speedUp.isDown(inputState);
    const bool descend = config->descend.isDown(inputState);
    const bool ascend = config->ascend.isDown(inputState);
    const bool resetMoveSpeed = config->resetMoveSpeed.isTapped(inputState);
    const bool decreaseMoveSpeed = config->decreaseMoveSpeed.isDown(inputState);
    const bool increaseMoveSpeed = config->increaseMoveSpeed.isDown(inputState);
    const bool resetRotation = config->resetRotation.isTapped(inputState);
    const bool rotateLeft = config->rotateLeft.isDown(inputState);
    const bool rotateRight = config->rotateRight.isDown(inputState);
    const bool resetFieldOfView = config->resetFieldOfView.isTapped(inputState);
    const bool decreaseFieldOfView = config->decreaseFieldOfView.isDown(inputState);
    const bool increaseFieldOfView = config->increaseFieldOfView.isDown(inputState);

    if (resetRotation)
    {
        rotation.setIdentity();
    }
    else
    {
        float rotateSpeed = config->rotateSpeed * factor;
        if (slowDown || speedUp)
            rotateSpeed *= speedUp ? config->rotateSpeedSpeedUpMultiplier : config->rotateSpeedSlowDownMultiplier;

        const float pitch = inputState->rightStickVertical * rotateSpeed;
        const float yaw = inputState->rightStickHorizontal * rotateSpeed;
        const float roll = (rotateLeft ? -rotateSpeed : rotateRight ? +rotateSpeed : 0.0f);

        Eigen::Vector3f yawAxis = Eigen::Vector3f::UnitY();
        if (!config->useFixedYawAxis)
            yawAxis = rotation * yawAxis;

        const Eigen::AngleAxisf x(DEGREES_TO_RADIANS(pitch), rotation * Eigen::Vector3f::UnitX());
        const Eigen::AngleAxisf y(DEGREES_TO_RADIANS(-yaw), yawAxis);
        const Eigen::AngleAxisf z(DEGREES_TO_RADIANS(roll), rotation * Eigen::Vector3f::UnitZ());

        rotation = (x * y * z * rotation).normalized();
    }

    if (resetMoveSpeed || increaseMoveSpeed || decreaseMoveSpeed)
        moveSpeed = max(0.01f, resetMoveSpeed ? config->moveSpeed : (moveSpeed + (increaseMoveSpeed ? config->moveSpeedIncreaseRatio : config->moveSpeedDecreaseRatio)));

    float currentMoveSpeed = moveSpeed * factor;
    if (slowDown || speedUp)
        currentMoveSpeed *= speedUp ? config->moveSpeedSpeedUpMultiplier : config->moveSpeedSlowDownMultiplier;

    const Eigen::Vector3f frontDirection = (rotation * -Eigen::Vector3f::UnitZ()).normalized();
    const Eigen::Vector3f rightDirection = (rotation * Eigen::Vector3f::UnitX()).normalized();
    const Eigen::Vector3f upDirection = Eigen::Vector3f::UnitY();

    position += (frontDirection * inputState->leftStickVertical + rightDirection * inputState->leftStickHorizontal + upDirection * (ascend ? config->descendAscendSpeed : descend ? -config->descendAscendSpeed : 0.0f)) * currentMoveSpeed;

    if (resetFieldOfView || increaseFieldOfView || decreaseFieldOfView)
        fieldOfView = fmodf(resetFieldOfView ? config->defaultFieldOfView : fieldOfView + (increaseFieldOfView ? config->fieldOfViewIncreaseRatio : config->fieldOfViewDecreaseRatio), 180.0f);

    camera->viewMatrix = (Eigen::Translation3f(position) * rotation).inverse().matrix();
    camera->fieldOfView = DEGREES_TO_RADIANS(fieldOfView * (16.0f / 9.0f / aspectRatio));
    camera->projectionMatrix = Eigen::CreatePerspectiveMatrix(camera->fieldOfView, aspectRatio, camera->zNear, camera->zFar);
    camera->position = position;
    camera->direction = frontDirection;
    camera->aspectRatio = aspectRatio;
}

void FreeCamera::reset()
{
    Eigen::Affine3f viewMatrix;
    viewMatrix = camera->viewMatrix.inverse();

    position = viewMatrix.translation();
    rotation = Eigen::Quaternionf(viewMatrix.rotation()).normalized();
    moveSpeed = config->moveSpeed;
    fieldOfView = config->defaultFieldOfView;
    aspectRatio = (float)*WIDTH / *HEIGHT;
}

FreeCamera::FreeCamera(Configuration* config, Camera* camera) : config(config), camera(camera)
{
    reset();
}
