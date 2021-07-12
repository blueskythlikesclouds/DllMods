#pragma once

struct Camera
{
    // Hedgehog::Universe::TStateMachine<Sonic::CCamera>
    // Sonic::CCamera
    uint8_t gap0[0x110];

    // Hedgehog::Mirage::CCamera
    // Sonic::CCamera::CMyCamera
    void* vtable110; // 0x110

    uint8_t gap114[12]; // 0x114

    Eigen::Matrix4f viewMatrix; // 0x120
    Eigen::Matrix4f projectionMatrix; // 0x160
    Eigen::Vector3f position; // 0x1A0
    uint32_t gap1AC;
    Eigen::Vector3f direction; // 0x1B0
    uint32_t gap1BC;
    float aspectRatio; // 0x1C0
    float zNear; // 0x1C4
    float zFar; // 0x1C8
    uint32_t gap1CC;
    Eigen::Matrix4f inputViewMatrix;

    uint8_t gap210[0x228];
    float fieldOfView; // 0x438
    uint8_t gap43C[0x2C];
    uint32_t field468;
    uint8_t gap46C[0x24];
};