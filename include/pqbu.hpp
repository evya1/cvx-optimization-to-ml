#pragma once

#include <Eigen/Dense>
#include <stdexcept>
#include "v_from_v.hpp"     // your implementation

namespace detail {
    template <typename Derived>
    // void validate_inputs(const Derived& p, const Derived& q, const Derived& z) {
    //     if (p.isZero() || q.isZero()) {
    //         throw std::invalid_argument("p and q must be non-zero.");
    //     }
    //     if (p.cross(q).isZero()) {
    //         throw std::invalid_argument("p and q must be linearly independent.");
    //     }
    // }

    void validate_pq_inputs(const Derived& p, const Derived& q) {
        if (p.isZero() || q.isZero()) {
            throw std::invalid_argument("p and q must be non-zero.");
        }
        if (p.cross(q).isZero()) {
            throw std::invalid_argument("p and q must be linearly independent.");
        }
    }


    template <typename Vec3>
    Eigen::Matrix<typename Vec3::Scalar, 3, 3>
    skew_symmetric(const Vec3& v) {
        Eigen::Matrix<typename Vec3::Scalar, 3, 3> M;
        M <<     0, -v.z(),  v.y(),
              v.z(),     0, -v.x(),
             -v.y(),  v.x(),     0;
        return M;
    }
}

/**
 * @brief Implements Algorithm 3 (PQBU) from algs2.pdf.
 *
 * Given three 3D vectors p, q, and z that define a special alignment frame,
 * this function computes the matrices (P, Q, B, U) and vector u used in special alignment (SA).
 *
 * The result encodes the geometry of aligning the point z with the span defined by p and q.
 * This is the preprocessing step before distance minimization in the FG pipeline (Algorithm 4).
 *
 * Preconditions:
 * - p ≠ 0
 * - q ≠ 0
 * - p and q must be linearly independent (i.e., q ∉ span{p})
 *
 * @tparam Derived Eigen column vector type (e.g., Eigen::Vector3d)
 * @param p Vector p ∈ ℝ³
 * @param q Vector q ∈ ℝ³ (linearly independent of p)
 * @param z Point z ∈ ℝ³
 *
 * @return std::tuple<P, Q, B, U, u> where:
 *         - P, Q, B ∈ ℝ^{3×2} (matrices from v_from_v)
 *         - U ∈ ℝ^{3×2} (skew-corrected update matrix)
 *         - u ∈ ℝ³ (scaled normal to the span{p, q})
 *
 * @throws std::invalid_argument if inputs are invalid (zero or collinear).
 *
 * @example
 * Eigen::Vector3d p(1, 0, 0), q(0, 1, 0), z(0.5, 0.5, 1.0);
 * auto [P, Q, B, U, u] = pqbu(p, q, z);
 */
template <typename Derived>
auto pqbu(const Derived& p, const Derived& q, const Derived& z)
-> std::tuple<
    typename EigenTraits<Derived>::MatD,
    typename EigenTraits<Derived>::MatD,
    typename EigenTraits<Derived>::MatD,
    typename EigenTraits<Derived>::MatD,
    Derived
>
{
    using T   = EigenTraits<Derived>;
    using Vec = typename T::VecD;
    using Mat = typename T::MatD;

    // Step 1: Validate inputs
    detail::validate_inputs(p, q, z);

    // Step 2: Project z onto affine subspace q + span{p - q}
    // Project z onto the affine subspace defined by q + span{p - q}
    // Resulting point b lies in the plane of p and q, closest to z
    Vec pq = p - q;
    Vec zq = z - q;
    Vec b  = q + (pq.dot(zq) / pq.squaredNorm()) * pq;

    // Step 3: Compute P, Q, B via v_from_v
    Mat P = v_from_v(p, q, p);
    Mat Q = v_from_v(p, q, q);
    // Mat B = v_from_v(p, q, b)
    Mat B = v_from_v(p, q, Derived(b));


    // Step 4: Compute cross vector v = p × q
    Vec v = p.cross(q);

    // Step 5: Skew-symmetric matrix [v]_× using inline construction
    // Build the 3×3 skew-symmetric matrix [v]_× used for cross product as matrix multiplication
    Mat v_hat = detail::skew_symmetric(v);

    // Step 6: Compute U = – (‖z–b‖/‖p–q‖) · [v]_× · (P – Q)
    // Scale and rotate the difference (P - Q) to construct the update matrix U
    double scale = (z - b).norm() / pq.norm();
    Mat U = -scale * v_hat * (P - Q);

    // Step 7: Compute u = ‖z–b‖ · v
    Vec u = (z - b).norm() * v;

    return {P, Q, B, U, u};
}
