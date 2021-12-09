#pragma once

// boost
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

// Eigen
#include <Eigen/Eigen>
#include <unsupported/Eigen/AlignedVector3>

// Helpers
#include <Helpers.h>

// Devil's Details' bullshit
#include <DX_PATCH.h>

// Sonicteam::System
namespace Sonicteam::System
{
    using Matrix = Eigen::Affine3f;
    using Matrix44 = Eigen::Matrix4f;
    using Quaternion = Eigen::Quaternionf;
    using Vector3 = Eigen::AlignedVector3<float>;
    using Vector4 = Eigen::Vector4f;
}

// Hedgehog::Math
namespace Hedgehog::Math
{
    using CAabb = Eigen::AlignedBox3f;
    using CMatrix = Sonicteam::System::Matrix;
    using CMatrix44 = Sonicteam::System::Matrix44;
    using CQuaternion = Sonicteam::System::Quaternion;
    using CVector = Sonicteam::System::Vector3;
    using CVector4 = Sonicteam::System::Vector4;
}

template<int n> struct InsertPadding
{
    INSERT_PADDING(n);
};

struct null_ctor{};