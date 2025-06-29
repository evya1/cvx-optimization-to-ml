#pragma once

#include <Eigen/Dense>
#include <cmath>
#include "rotate.hpp"

// Generic type aliases
template <typename Scalar>
using VecD = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;

template <typename Scalar>
using MatD = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;

template <typename Scalar>
using Vec2 = Eigen::Matrix<Scalar, 2, 1>;

template <typename Scalar>
using Mat2 = Eigen::Matrix<Scalar, 2, 2>;

// Traits for deducing Scalar and applying aliases inside templated functions
template <typename Derived>
struct EigenTraits {
    using Scalar = typename Derived::Scalar;
    using VecD   = ::VecD<Scalar>;
    using MatD   = ::MatD<Scalar>;
    using Vec2   = ::Vec2<Scalar>;
    using Mat2   = ::Mat2<Scalar>;
};

/**
 * @brief Constructs matrix V(v) ∈ ℝᵈ×² as defined in Algorithm 2.
 *
 * Accepts both fixed-size and dynamic column vectors p, q, v ∈ ℝᵈ.
 * Computes a matrix V ∈ ℝᵈ×² such that for any R ∈ SO(p,q), the translation
 * t = V(v)g(R) - Rv ensures alignment [R | t] ∈ SOA(p,q).
 *
 * @return Matrix of size d×2.
 */
template <typename Derived>
auto v_from_v(const Derived& p, const Derived& q, const Derived& v)
-> typename EigenTraits<Derived>::MatD
{
    using T = EigenTraits<Derived>;
    using Scalar = typename T::Scalar;

    const int d = p.rows();
    if (d != q.rows() || d != v.rows() || p.cols() != 1 || q.cols() != 1 || v.cols() != 1)
        throw std::invalid_argument("All inputs must be column vectors of the same dimension.");

    typename T::VecD p_norm = p.normalized();
    typename T::VecD proj_q = p_norm * (p_norm.dot(q));
    typename T::VecD q_orth = q - proj_q;

    Scalar q_orth_norm = q_orth.norm();
    if (q_orth_norm == Scalar(0))
        throw std::invalid_argument("Vectors p and q must be linearly independent.");
    q_orth /= q_orth_norm;

    typename T::MatD F(d, 2);
    F << p_norm, q_orth;

    typename T::Vec2 p2 = F.transpose() * p;
    typename T::Vec2 q2 = F.transpose() * q;
    typename T::Vec2 v2 = F.transpose() * v;

    // FIX: Removed `typename` before static function call `Zero()`
    typename T::Mat2 U = T::Mat2::Zero();
    if (!v2.isApprox(q2))
        U = rotate(p2 - q2, v2 - q2);

    Scalar c = (v2 - q2).norm() / (p2 - q2).norm();

    typename T::Mat2 M;

    M.col(0) = c * U * p2;
    M.col(1) = (c * U - T::Mat2::Identity()) * q2;

    typename T::Mat2 PQ;
    PQ << p2, q2;

    return (p - q).norm() * F * M * PQ.inverse();
}