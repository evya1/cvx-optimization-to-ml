#pragma once

#include <Eigen/Dense>
#include <stdexcept>

template <typename Scalar>
using Vec2 = Eigen::Matrix<Scalar, 2, 1>;
template <typename Scalar>
using Mat2 = Eigen::Matrix<Scalar, 2, 2>;

/**
 * Constructs the unique SO(2) rotation matrix that maps the direction of vector w to that of vector u.
 * @param w Input 2D vector to rotate from.
 * @param u Input 2D vector to rotate to.
 * @return The SO(2) rotation matrix.
 * @throws std::invalid_argument if either input vector is zero and cannot be normalized.
 */
template <typename Scalar>
Mat2<Scalar> rotate(const Vec2<Scalar>& w, const Vec2<Scalar>& u)
{

    // Use an appropriate epsilon for floating-point comparison
    constexpr Scalar epsilon = Scalar(1e-10);
    if (w.isZero(epsilon) || u.isZero(epsilon))
    {
        throw std::invalid_argument("Input vector is zero; cannot normalize.");
    }

    // Step 1: Normalize both w,u
    Vec2<Scalar> w_norm = w.normalized();
    Vec2<Scalar> u_norm = u.normalized();

    // Step 2: Rotation matrix J for R^2
    Mat2<Scalar> J;
    J << Scalar(0), Scalar(-1),
        Scalar(1), Scalar(0);

    // Step 3: Form W and U matrices
    Mat2<Scalar> u_Ju, w_Jw;
    u_Ju << u_norm, J * u_norm;
    w_Jw << w_norm, J * w_norm;

    // Step 4: Return rotation matrix U
    Mat2<Scalar> U = u_Ju * w_Jw.transpose();
    return U;
}
