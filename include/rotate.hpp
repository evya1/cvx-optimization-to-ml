#pragma once

#include <Eigen/Dense>
#include <stdexcept>

// Private Implementation Namespace for Helper Functions

namespace detail {

/**
 * @brief Validates that the input 2D vectors are non-zero.
 */
template <typename DerivedW, typename DerivedU>
void validate_are_non_zero(const Eigen::MatrixBase<DerivedW>& w, const Eigen::MatrixBase<DerivedU>& u) {
    if (w.isZero() || u.isZero()) {
        throw std::invalid_argument("Input vectors cannot be zero.");
    }
}

/**
 * @brief Creates a 2x2 orthonormal basis matrix from a single 2D unit vector.
 *
 * Given a unit vector `v`, this function constructs the matrix `[v, J*v]`,
 * where `J` is the 2D matrix for a 90-degree counter-clockwise rotation.
 * The resulting matrix is guaranteed to be in SO(2).
 */
template <typename Derived>
auto create_rotation_basis_from_vector(const Eigen::MatrixBase<Derived>& v)
-> Eigen::Matrix<typename Derived::Scalar, 2, 2>
{
    using Scalar = typename Derived::Scalar;
    using M2 = Eigen::Matrix<Scalar, 2, 2>;

    // Define the 90-degree rotation matrix J
    M2 J;
    J << Scalar(0), Scalar(-1),
         Scalar(1), Scalar(0);

    // Form the basis matrix [v, J*v]
    M2 basis;
    basis << v, J * v;

    return basis;
}

}


/**
 * @brief Returns the unique 2×2 rotation matrix that maps one direction vector to another.
 */
template <typename DerivedW, typename DerivedU>
auto rotate(const Eigen::MatrixBase<DerivedW>& w,
            const Eigen::MatrixBase<DerivedU>& u)
-> Eigen::Matrix<typename DerivedW::Scalar, 2, 2>
{
    // Step 1: Ensure input vectors are valid.
    detail::validate_are_non_zero(w, u);

    // Step 2: Normalize the direction vectors.
    const auto w_norm = w.normalized();
    const auto u_norm = u.normalized();

    // Step 3: Create orthonormal bases from each unit vector.
    // The matrix W_basis maps the standard basis to the frame defined by w_norm.
    // The matrix U_basis maps the standard basis to the frame defined by u_norm.
    auto W_basis = detail::create_rotation_basis_from_vector(w_norm);
    auto U_basis = detail::create_rotation_basis_from_vector(u_norm);

    // Step 4: Compute the rotation from W's frame to U's frame.
    // The result U = U_basis * W_basis_inverse = U_basis * W_basis_transpose
    // is the rotation that aligns w_norm with u_norm.
    return U_basis * W_basis.transpose();
}
