#pragma once

#include <Eigen/Dense>
#include <stdexcept>


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
 * @tparam DerivedW Eigen vector expression for `w`
 * @tparam DerivedU Eigen vector expression for `u`
 * @return Mat2<Scalar> rotation matrix in SO(2)
 * @throws std::invalid_argument if w or u is zero
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
template <typename DerivedW, typename DerivedU>
auto rotate(const Eigen::MatrixBase<DerivedW>& w,
            const Eigen::MatrixBase<DerivedU>& u)
-> Eigen::Matrix<typename DerivedW::Scalar, 2, 2>
{
    using Scalar = typename DerivedW::Scalar;
    using V2 = Eigen::Matrix<Scalar, 2, 1>;
    using M2 = Eigen::Matrix<Scalar, 2, 2>;;

    // Edge case for 0 vector input
    if (w.isZero() || u.isZero()) {
        throw std::invalid_argument("Input vector is zero; cannot normalize.");
    }


    // Step 1: Normalize both our vectors $w,u$
    V2 w_norm = w.normalized();
    V2 u_norm = u.normalized();

    // Step 2: Rotation matrix J for R^2
    M2 J;
    J << Scalar(0), Scalar(-1),
        Scalar(1), Scalar(0);

    // Step 3: Form W and U matrices
    M2 u_Ju, w_Jw;
    u_Ju << u_norm, J * u_norm;
    w_Jw << w_norm, J * w_norm;

    return u_Ju * w_Jw.transpose();
}
