#pragma once

#include <Eigen/Dense>
#include "rotate.hpp"
#include <stdexcept>
#include <limits>

// --- Type Aliases and Traits (Unchanged) ---
template <typename Scalar>
using VecD = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;
template <typename Scalar>
using MatD = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;
template <typename Scalar>
using Vec2 = Eigen::Matrix<Scalar, 2, 1>;
template <typename Scalar>
using Mat2 = Eigen::Matrix<Scalar, 2, 2>;

template <typename Derived>
struct EigenTraits {
    using Scalar = typename Derived::Scalar;
    using VecD   = ::VecD<Scalar>;
    using MatD   = ::MatD<Scalar>;
    using Vec2   = ::Vec2<Scalar>;
    using Mat2   = ::Mat2<Scalar>;
};

// --- Private Implementation Namespace for Helper Functions ---
namespace detail {

/**
 * Validates that all input vectors are non-zero column vectors of the same dimension,
 * and that p and q are linearly independent.
 */
template <typename Derived>
void validate_inputs(const Derived& p, const Derived& q, const Derived& v) {
    using Scalar = typename Derived::Scalar;
    const int d = p.rows();

    if (d != q.rows() || d != v.rows() || p.cols() != 1 || q.cols() != 1 || v.cols() != 1) {
        throw std::invalid_argument("All inputs must be column vectors of the same dimension.");
    }
    if (p.norm() < std::numeric_limits<Scalar>::epsilon() || q.norm() < std::numeric_limits<Scalar>::epsilon()) {
        throw std::invalid_argument("Input vectors p and q must be non-zero.");
    }
    // Check for linear independence by ensuring the component of q orthogonal to p is non-zero.
    if ((q - p.normalized() * (p.normalized().dot(q))).norm() < std::numeric_limits<Scalar>::epsilon()) {
        throw std::invalid_argument("Vectors p and q must be linearly independent.");
    }
}

/**
 * Uses the Gram-Schmidt process to create an orthonormal basis F from vectors p and q.
 */
template <typename T, typename Derived>
auto create_orthonormal_basis(const Derived& p, const Derived& q) -> typename T::MatD {
    // p and q have already been validated as linearly independent and non-zero.
    typename T::VecD p_norm = p.normalized();
    typename T::VecD q_orth = (q - p_norm * (p_norm.dot(q))).normalized();

    typename T::MatD F(p.rows(), 2);
    F << p_norm, q_orth;
    return F;
}

/**
 * Computes the 2D alignment matrix M based on the projected 2D vectors.
 * This encapsulates the core 2D logic of the algorithm.
 */
template <typename T>
auto compute_2d_alignment_matrix(const typename T::Vec2& p_2d, const typename T::Vec2& q_2d, const typename T::Vec2& v_2d) -> typename T::Mat2 {
    // Find the 2D rotation U that aligns (v_2d - q_2d) to (p_2d - q_2d).
    // If v_2d is already aligned with q_2d, no rotation is needed.
    typename T::Mat2 U_2d = T::Mat2::Identity();
    if (!v_2d.isApprox(q_2d)) {
        U_2d = rotate(p_2d - q_2d, v_2d - q_2d);
    }

    // Calculate the scaling factor 'c' and assemble the intermediate matrix M.
    const auto c = (v_2d - q_2d).norm() / (p_2d - q_2d).norm();

    typename T::Mat2 M_2d;
    M_2d.col(0) = c * U_2d * p_2d;
    M_2d.col(1) = (c * U_2d - T::Mat2::Identity()) * q_2d;

    return M_2d;
}

} // namespace detail

/**
 * @brief Constructs matrix V(v) ∈ ℝ^(dx2) as defined in Algorithm 2.
 * (Full Doxygen documentation remains the same)
 */
template <typename Derived>
auto v_from_v(const Derived& p, const Derived& q, const Derived& v)
-> typename EigenTraits<Derived>::MatD
{
    using T = EigenTraits<Derived>;

    // Step 1: Validate all inputs before proceeding.
    detail::validate_inputs(p, q, v);

    // Step 2: Create an orthonormal basis F for the plane span{p, q}.
    auto F = detail::create_orthonormal_basis<T>(p, q);
    auto FT = F.transpose();

    // Step 3: Project the d-dimensional vectors into the 2D basis F.
    const auto p_2d = FT * p;
    const auto q_2d = FT * q;
    const auto v_2d = FT * v;

    // Step 4: Solve the 2D alignment problem to get the intermediate matrix M.
    auto M_2d = detail::compute_2d_alignment_matrix<T>(p_2d, q_2d, v_2d);

    // Step 5: Construct the final matrix V by mapping the 2D solution back to d-dimensions.
    const auto PQ_2d_inv = (typename T::Mat2() << p_2d, q_2d).finished().inverse();
    const auto scale = (p - q).norm();

    return scale * F * M_2d * PQ_2d_inv;
}