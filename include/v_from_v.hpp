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
 * @brief Constructs matrix V(v) ∈ ℝ^(dx2) as defined in Algorithm 2.
 *
 * This function computes a matrix V ∈ ℝᵈ×² based on three vectors p, q, and v.
 * The resulting matrix has a special property: for any rotation R in the plane
 * defined by p and q, the translation vector t = V(v)g(R) - Rv ensures
 * that the transformation [R | t] aligns the vectors correctly according to
 * the Special Orthogonal Alignment (SOA) problem.
 *
 * @param p The first basis column vector of the plane, p ∈ ℝᵈ.
 * @param q The second basis column vector of the plane, q ∈ ℝᵈ. Must be linearly independent from p.
 * @param v The target column vector, v ∈ ℝᵈ, which lies in the span of p and q.
 *
 * @return A matrix V of size d×2, with the same scalar type as the inputs.
 *         Throws std::invalid_argument if inputs are not column vectors of the
 *         same dimension, or if p and q are linearly dependent.
 *
 * @example
 * // In 3D, define p, q, v
 * Eigen::Vector3d p(1, 0, 0);
 * Eigen::Vector3d q(0, 1, 0);
 * Eigen::Vector3d v = p; // A simple case where v is p
 *
 * // Compute the V matrix
 * Eigen::Matrix<double, 3, 2> V = v_from_v(p, q, v);
 *
 * // V will now be a 3x2 matrix that can be used for alignment calculations.
 */
template <typename Derived>
auto v_from_v(const Derived& p, const Derived& q, const Derived& v)
-> typename EigenTraits<Derived>::MatD
{
    using T = EigenTraits<Derived>;
    using Scalar = typename T::Scalar;

    const int d = p.rows();
    const bool ValidInputDim = d != q.rows() || d != v.rows() || p.cols() != 1 || q.cols() != 1 || v.cols() != 1;
    if (ValidInputDim)
        throw std::invalid_argument("All inputs must be column vectors of the same dimension.");

    // Step 1: Create an orthonormal basis F for plane sp{p, q}.
    typename T::VecD p_norm = p.normalized();
    typename T::VecD proj_q = p_norm * (p_norm.dot(q));
    typename T::VecD q_orth = q - proj_q;

    // Ensure p and q are not collinear by checking the norm of the orthogonal part.
    Scalar q_orth_norm = q_orth.norm();
    auto ZeroNorm = q_orth_norm < std::numeric_limits<Scalar>::epsilon();
    if (ZeroNorm)
        throw std::invalid_argument("Vectors p and q must be linearly independent.");

    // Normalize the orthogonal part to get the second basis vector.
    q_orth /= q_orth_norm;

    // The orthonormal basis F is a d×2 matrix.
    typename T::MatD F(d, 2);
    F << p_norm, q_orth;

    // Step 2: Project the d-dimensional vectors into the 2D basis F.
    const auto FT = F.transpose();

    typename T::Vec2 p2 = FT * p;
    typename T::Vec2 q2 = FT * q;
    typename T::Vec2 v2 = FT * v;

    // Step 3: Compute the 2D rotation matrix U that aligns (v2 - q2) with (p2 - q2).
    typename T::Mat2 U = T::Mat2::Identity();
    auto v2_inequal_q2 = !v2.isApprox(q2);
    if (v2_inequal_q2) {
        U = rotate(p2 - q2, v2 - q2);
    }

    // Step 4: Calculate the scaling factor 'c' and the intermediate matrix M.
    Scalar c = (v2 - q2).norm() / (p2 - q2).norm();

    typename T::Mat2 M;
    // The columns of M are derived from the alignment logic in 2D.
    M.col(0) = c * U * p2;
    M.col(1) = (c * U - T::Mat2::Identity()) * q2;

    // The 2x2 matrix [p2, q2] represents the change of basis matrix in 2D.
    typename T::Mat2 PQ;
    PQ << p2, q2;


    // Step 5: Construct the final matrix V by mapping the 2D solution back to d-dimensions.
    // The final result is scaled by the norm of (p-q) as per the algorithm's definition.
    auto V = (p - q).norm() * F * M * PQ.inverse();
    return V;
}