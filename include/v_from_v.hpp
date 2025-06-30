#pragma once

#include <Eigen/Dense>
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
    const auto has_invalid_input_dimensions = d != q.rows() || d != v.rows() || p.cols() != 1 || q.cols() != 1 || v.cols() != 1;
    if (has_invalid_input_dimensions)
        throw std::invalid_argument("All inputs must be column vectors of the same dimension.");

    // Step 1: Create an orthonormal basis F for plane sp{p, q}.
    const auto is_p_zero_vector = p.norm() < std::numeric_limits<Scalar>::epsilon();
    if (is_p_zero_vector)
        throw std::invalid_argument("Vector p must be non-zero.");
    auto p_norm = p.normalized();
    typename T::VecD proj_q = p_norm * (p_norm.dot(q));
    typename T::VecD q_orth = q - proj_q;

    // Ensure p and q are not collinear by checking the norm of the orthogonal part.
    const auto q_orth_norm = q_orth.norm();
    const auto is_q_orthogonal_zero = q_orth_norm < std::numeric_limits<Scalar>::epsilon();
    if (is_q_orthogonal_zero)
        throw std::invalid_argument("Vectors p and q must be linearly independent.");

    // Normalize the orthogonal part to get the second basis vector.
    q_orth /= q_orth_norm;

    // The orthonormal basis F is a d×2 matrix.
    typename T::MatD F(d, 2);
    F << p_norm, q_orth;

    // Step 2: Project the d-dimensional vectors into the 2D basis F.
    const auto FT = F.transpose();
    const auto p_2d = FT * p;
    const auto q_2d = FT * q;
    const auto v_2d = FT * v;

    // Step 3: Compute the 2D rotation matrix U that aligns (v2 - q2) with (p2 - q2).
    typename T::Mat2 rotation_2d = T::Mat2::Identity();
    const auto is_v2_distinct_from_q2 = !v_2d.isApprox(q_2d);
    if (is_v2_distinct_from_q2) {
        rotation_2d = rotate(p_2d - q_2d, v_2d - q_2d);
    }

    // Step 4: Calculate the scaling factor 'c' and the intermediate matrix M.
    const auto c = (v_2d - q_2d).norm() / (p_2d - q_2d).norm();

    typename T::Mat2 alignment_matrix;
    alignment_matrix.col(0) = c * rotation_2d * p_2d;
    alignment_matrix.col(1) = (c * rotation_2d - T::Mat2::Identity()) * q_2d;

    // The 2x2 matrix [p2, q2] represents the change of basis matrix in 2D.
    const auto change_of_basis = (typename T::Mat2() << p_2d, q_2d).finished();


    // Step 5: Construct the final matrix V by mapping the 2D solution back to d-dimensions.
    // The final result is scaled by the norm of (p-q) as per the algorithm's definition.
    const auto V = (p - q).norm() * F * alignment_matrix * change_of_basis.inverse();
    return V;
}