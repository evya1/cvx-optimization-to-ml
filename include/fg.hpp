#pragma once

#include <Eigen/Dense>
#include "rotate.hpp"
#include "pqbu.hpp"

template <typename Derived>
auto fg(const Derived& p, const Derived& q, const Derived& z, const Derived& ell)
-> std::tuple<
    Eigen::Matrix<typename Derived::Scalar, 2, 2>,
    Eigen::Matrix<typename Derived::Scalar, 2, 2>,
    typename Derived::Scalar
>
{
    using T = EigenTraits<Derived>;
    using Scalar = typename T::Scalar;
    using Vec = typename T::VecD;
    using Mat = typename T::MatD;
    using Mat2 = typename T::Mat2;


    // Step 1: Normalize ℓ (using fixed-size for 3D ops)
    using Vec3 = Eigen::Matrix<Scalar, 3, 1>;
    Vec3 ell_unit = ell.normalized();

    // Step 2: Construct orthonormal basis L such that Lᵗℓ = 0, LᵗL = I
    Vec3 ell_perp;
    if (std::abs(ell_unit(0)) < 1e-12 && std::abs(ell_unit(1)) < 1e-12) {
        ell_perp = Vec3(0, 1, 0); // (0,1,0)
    } else {
        ell_perp = Vec3(-ell_unit(1), ell_unit(0), 0.0).normalized();
    }
    Vec3 ell_cross = ell_unit.cross(ell_perp).normalized();
    Mat L(3, 2);
    L.col(0) = ell_perp;
    L.col(1) = ell_cross;

    // Step 3: Call PQBU
    auto [P, Q, B, U, u] = pqbu(p, q, z);

    // Step 4: Project B, U, u into ℓ⊥ plane using Lᵗ
    Mat2 B_proj = (L.transpose() * B).eval();
    Mat2 U_proj = (L.transpose() * U).eval();
    Vec u_proj = L.transpose() * u;
    Scalar c = u_proj.norm();

    // Step 5: Compute rotation H ∈ SO(2) that maps u_proj to (0,1)
    Mat2 H = Mat2::Identity();
    if (c > Scalar(1e-12)) {
        Eigen::Vector2d target(0.0, 1.0);
        H = rotate(u_proj, target);
    }

    // Step 6: Return F = H * B_proj, G = H * U_proj
    Mat2 F = H * B_proj;
    Mat2 G = H * U_proj;

    return {F, G, c};
}
