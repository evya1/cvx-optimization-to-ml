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
    // Check for zero-length vectors to avoid division by zero.
    // Eigen::NumTraits provides a safe, type-aware epsilon.
    if (w.squaredNorm() < Eigen::NumTraits<Scalar>::dummy_precision() ||
        u.squaredNorm() < Eigen::NumTraits<Scalar>::dummy_precision())
    {
        throw std::invalid_argument("Input vector is zero; cannot normalize.");
    }

    // Step 1: Normalize both vectors
    Vec2<Scalar> w_norm = w.normalized();
    Vec2<Scalar> u_norm = u.normalized();

    // Step 2: The cosine of the angle is the dot product
    Scalar c = w_norm.dot(u_norm);

    // Step 3: The sine of the angle is the 2D cross product
    Scalar s = w_norm[0] * u_norm[1] - w_norm[1] * u_norm[0];

    // Step 4: Construct the rotation matrix directly
    Mat2<Scalar> R;
    R << c, -s,
         s,  c;
    return R;
}
