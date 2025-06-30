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
    Mat v_hat = detail::skew_symmetric(v);

    // Step 6: Compute U = – (‖z–b‖/‖p–q‖) · [v]_× · (P – Q)
    double scale = (z - b).norm() / pq.norm();
    Mat U = -scale * v_hat * (P - Q);

    // Step 7: Compute u = ‖z–b‖ · v
    Vec u = (z - b).norm() * v;

    return {P, Q, B, U, u};
}
