#pragma once

#include <Eigen/Dense>
#include <stdexcept>
#include <cmath>
#include "rotate.hpp"

template <typename Scalar>
using VecD = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;

template <typename Scalar>
using MatD = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;

template <typename Scalar>
using Vec2 = Eigen::Matrix<Scalar, 2, 1>;

template <typename Scalar>
using Mat2 = Eigen::Matrix<Scalar, 2, 2>;


/**
 * @brief Constructs matrix V(v) ∈ ℝᵈ×² as defined in Algorithm 2.
 *
 * Given p, q ∈ ℝᵈ (non-zero and linearly independent), and a point v ∈ sp{p, q}, this algorithm returns
 * a matrix V(v) ∈ ℝᵈ×² such that for any R ∈ SO(p, q), the translation t = V(v)g(R) - Rv maps
 * the alignment [R | t] ∈ SOA(p, q).
 *
 * @param p Non-zero vector in ℝ^d.
 * @param q Vector not in the span of p.
 * @param v A vector in the plane spanned by {p, q}.
 * @return Matrix of size d×2 defining the affine map for the translation t.
 */
template <typename Scalar>
MatD<Scalar> v_from_v(const VecD<Scalar>& p, const VecD<Scalar>& q, const VecD<Scalar>& v)
{
    using std::sqrt;
    const int d = p.size();
    if (d != q.size() || d != v.size()) {
        throw std::invalid_argument("Vectors p, q, and v must have the same dimension.");
    }

    // Step 1: Orthonormal columns for matrix F ∈ ℝᵈ×²
    VecD<Scalar> p_norm = p.normalized();
    VecD<Scalar> proj_q_on_p = p_norm * (p_norm.dot(q));

    VecD<Scalar> q_orth = q - proj_q_on_p;
    Scalar norm_q_orth = q_orth.norm();

    if (norm_q_orth == Scalar(0)) {
        throw std::invalid_argument("Vectors p and q must be linearly independent.");
    }
    q_orth /= norm_q_orth;

    MatD<Scalar> F(d, 2);
    F.col(0) = p_norm;
    F.col(1) = q_orth;

    // Step 2: Project p, q, v into 2D
    Vec2<Scalar> p_prime = F.transpose() * p;
    Vec2<Scalar> q_prime = F.transpose() * q;
    Vec2<Scalar> v_prime = F.transpose() * v;

    // Step 3: Construct U ∈ SO(2) or zero
    Mat2<Scalar> U = Mat2<Scalar>::Zero();
    if (!v_prime.isApprox(q_prime)) {
        Vec2<Scalar> w = p_prime - q_prime;
        Vec2<Scalar> u = v_prime - q_prime;
        U = rotate(w, u);
    }

    // Step 4: Compute scalar c
    Scalar c = (v_prime - q_prime).norm() / (p_prime - q_prime).norm();

    // Step 5: Construct final V(v) matrix
    Mat2<Scalar> M;
    M.col(0) = c * U * p_prime;
    M.col(1) = (c * U - Mat2<Scalar>::Identity()) * q_prime;

    Mat2<Scalar> pq_prime;
    pq_prime.col(0) = p_prime;
    pq_prime.col(1) = q_prime;

    MatD<Scalar> V = (p - q).norm() * F * M * pq_prime.inverse();
    return V;
}
