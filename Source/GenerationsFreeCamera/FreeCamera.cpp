#include "FreeCamera.h"

const uint32_t* const WIDTH = (uint32_t*)0x1DFDDDC;
const uint32_t* const HEIGHT = (uint32_t*)0x1DFDDE0;

void FreeCamera::update(const Sonic::SPadState& padState, const float elapsedTime)
{
    const float factor = elapsedTime / (1.0f / 60.0f);

    const bool slowDown = config->slowDown.isDown(padState);
    const bool speedUp = config->speedUp.isDown(padState);
    const bool descend = config->descend.isDown(padState);
    const bool ascend = config->ascend.isDown(padState);
    const bool resetMoveSpeed = config->resetMoveSpeed.isTapped(padState);
    const bool decreaseMoveSpeed = config->decreaseMoveSpeed.isDown(padState);
    const bool increaseMoveSpeed = config->increaseMoveSpeed.isDown(padState);
    const bool resetRotation = config->resetRotation.isTapped(padState);
    const bool rotateLeft = config->rotateLeft.isDown(padState);
    const bool rotateRight = config->rotateRight.isDown(padState);
    const bool resetFieldOfView = config->resetFieldOfView.isTapped(padState);
    const bool decreaseFieldOfView = config->decreaseFieldOfView.isDown(padState);
    const bool increaseFieldOfView = config->increaseFieldOfView.isDown(padState);

    if (resetRotation)
    {
        rotation.setIdentity();
    }
    else
    {
        float rotateSpeed = config->rotateSpeed * factor;
        if (slowDown || speedUp)
            rotateSpeed *= speedUp ? config->rotateSpeedSpeedUpMultiplier : config->rotateSpeedSlowDownMultiplier;

        const float pitch = padState.RightStickVertical * rotateSpeed;
        const float yaw = padState.RightStickHorizontal * rotateSpeed;
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

    position += (frontDirection * padState.LeftStickVertical + rightDirection * padState.LeftStickHorizontal + upDirection * (ascend ? config->descendAscendSpeed : descend ? -config->descendAscendSpeed : 0.0f)) * currentMoveSpeed;

    if (resetFieldOfView || increaseFieldOfView || decreaseFieldOfView)
        fieldOfView = fmodf(resetFieldOfView ? config->defaultFieldOfView : fieldOfView + (increaseFieldOfView ? config->fieldOfViewIncreaseRatio : config->fieldOfViewDecreaseRatio) * elapsedTime * 60.0f, 180.0f);

    camera->m_MyCamera.m_View = (Eigen::Translation3f(position) * rotation).inverse().matrix();
    camera->m_MyCamera.m_InputView = camera->m_MyCamera.m_View;
    camera->m_FieldOfView = 2.0f * atan(tan(DEGREES_TO_RADIANS(fieldOfView / 2.0f) * (16.0f / 9.0f / min(aspectRatio, 16.0f / 9.0f))));
    camera->m_MyCamera.m_Projection = Eigen::CreatePerspectiveMatrix(camera->m_FieldOfView, aspectRatio, camera->m_MyCamera.m_Near, camera->m_MyCamera.m_Far);
    camera->m_MyCamera.m_Position = position;
    camera->m_MyCamera.m_Direction = frontDirection;
    // Setting aspect ratio has issues with the ultrawide code, so don't do it
    // camera->aspectRatio = aspectRatio;
}

void FreeCamera::reset()
{
    Eigen::Affine3f viewMatrix;
    viewMatrix = camera->m_MyCamera.m_View.inverse();

    position = viewMatrix.translation();
    rotation = Eigen::Quaternionf(viewMatrix.rotation()).normalized();
    moveSpeed = config->moveSpeed;
    fieldOfView = config->defaultFieldOfView;
    aspectRatio = (float)*WIDTH / (float)*HEIGHT;
}

FreeCamera::FreeCamera(Configuration* config, Sonic::CCamera* camera) : config(config), camera(camera)
{
    reset();
}
