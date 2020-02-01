#pragma once

#define DEGREES_TO_RADIANS(x) (float)(x / 180.0f * M_PI)
#define RADIANS_TO_DEGREES(x) (float)(x / M_PI * 180.0f)

namespace Eigen
{
    template<typename Scalar>
    Matrix<Scalar, 4, 4> CreatePerspectiveMatrix(const Scalar fieldOfView, const Scalar aspectRatio, const Scalar zNear, const Scalar zFar)
    {
        const Scalar yScale = (Scalar)1 / std::tan(fieldOfView / (Scalar)2);
        const Scalar xScale = yScale / aspectRatio;

        Matrix<Scalar, 4, 4> matrix;

        matrix <<
            xScale, 0, 0, 0,
            0, yScale, 0, 0,
            0, 0, -(zFar + zNear) / (zFar - zNear), -2 * zNear * zFar / (zFar - zNear),
            0, 0, -1, 0;

        return matrix;
    }
}
