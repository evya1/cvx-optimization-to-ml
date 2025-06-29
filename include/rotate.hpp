#pragma once

#include <Eigen/Dense>
#include <stdexcept>

template <typename Scalar>
using Vec2 = Eigen::Matrix<Scalar, 2, 1>;
template <typename Scalar>
using Mat2 = Eigen::Matrix<Scalar, 2, 2>;

/**
 * @brief Returns the unique 2×2 rotation matrix that maps one direction vector to another.
 *
 * Given two non-zero 2D vectors `w` and `u`, this function returns the unique matrix `U` ∈ SO(2)
 * (i.e., orthogonal with determinant 1) such that:
 *
 *     U · (w / ‖w‖) = u / ‖u‖
 *
 * It constructs a special orthogonal matrix that rotates `w` onto `u`,
 * preserving the orientation and direction in ℝ².
 *
 * @tparam Scalar A floating-point type such as `float` or `double`. Determines the precision.
 * @param w A non-zero vector of type `Vec2<Scalar>`. Treated as the source direction.
 * @param u A non-zero vector of type `Vec2<Scalar>`. Treated as the target direction.
 * @return A 2×2 matrix of type `Mat2<Scalar>`, belonging to SO(2), that rotates `w` to align with `u`.
 * @throws std::invalid_argument If `w` or `u` is the zero vector (i.e., direction is undefined).
 *
 * @example
 * Example: Rotate (1,0) to the direction of (3,3)
 *
 * ```cpp
 * Vec2d w(1, 0);     // Source direction
 * Vec2d u(3, 3);     // Target direction
 *
 * Vec2d w_unit = w.normalized();
 * Vec2d u_unit = u.normalized();
 *
 * Mat2d U = rotate(w_unit, u_unit);
 * Vec2d result = U * w_unit;
 *
 * // Now result ≈ u_unit
 * ```
 */
template <typename Scalar>
Mat2<Scalar> rotate(const Vec2<Scalar>& w, const Vec2<Scalar>& u)
{

    // Edge case for 0 vector input
    if (w.isZero() || u.isZero()) {
        throw std::invalid_argument("Input vector is zero; cannot normalize.");
    }


    // Step 1: Normalize both our vectors $w,u$
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
