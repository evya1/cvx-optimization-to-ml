#pragma once

#include <Eigen/Dense>
#include <stdexcept>

template <typename Scalar>
using Vec2 = Eigen::Matrix<Scalar, 2, 1>;
template <typename Scalar>
using Mat2 = Eigen::Matrix<Scalar, 2, 2>;

/**
* Returns the unique matrix U ∈ SO(2) ⊂ ℝ²×² such that:
 *     U ( w / ∥w∥ ) = u / ∥u∥
 *
 * Here, SO(2) denotes the group of 2×2 real rotation matrices:
 *     SO(2) := { R ∈ ℝ²×² | RᵀR = I, det(R) = 1 }.
 *
 * The rotation is constructed as:
 *     U := [û, Jû] · [ŵ, Jŵ]ᵀ
 * where ŵ := w / ∥w∥, û := u / ∥u∥, and J := [0 −1; 1 0].
 *
 * @tparam Scalar The scalar type (e.g., float, double).
 * @param w A non-zero 2D input vector to rotate from.
 * @param u A non-zero 2D input vector to rotate to.
 * @return A 2×2 special orthogonal matrix that rotates `w` to `u`.
 * @throws std::invalid_argument if `w` or `u` is the zero vector (norm == 0).
 *
 * @example
 *     Vec2d w(1, 0);       // x-axis
 *     Vec2d u(0, 1);       // y-axis
 *     Mat2d U = rotate(w, u);
 *     // U should be a 90-degree counterclockwise rotation matrix:
 *     // [
 *     //     0  -1
 *     //     1   0
 *     // ]
 *     Vec2d rotated = U * w.normalized();
 *     // rotated ≈ u.normalized() => [0, 1]
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

// template <typename Scalar>
// Mat2<Scalar> rotate(const Vec2<Scalar>& w, const Vec2<Scalar>& u)
// {
//     // Check for zero-length vectors to avoid division by zero.
//     // Eigen::NumTraits provides a safe, type-aware epsilon.
//     if (w.squaredNorm() < Eigen::NumTraits<Scalar>::dummy_precision() ||
//         u.squaredNorm() < Eigen::NumTraits<Scalar>::dummy_precision())
//     {
//         throw std::invalid_argument("Input vector is zero; cannot normalize.");
//     }
//
//     // Step 1: Normalize both vectors
//     Vec2<Scalar> w_norm = w.normalized();
//     Vec2<Scalar> u_norm = u.normalized();
//
//     // Step 2: The cosine of the angle is the dot product
//     Scalar c = w_norm.dot(u_norm);
//
//     // Step 3: The sine of the angle is the 2D cross product
//     Scalar s = w_norm[0] * u_norm[1] - w_norm[1] * u_norm[0];
//
//     // Step 4: Construct the rotation matrix directly
//     Mat2<Scalar> R;
//     R << c, -s,
//          s,  c;
//     return R;
// }
