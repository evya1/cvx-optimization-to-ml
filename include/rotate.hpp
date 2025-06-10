#pragma once

#include <Eigen/Dense>

template <typename Scalar>
using Vec2 = Eigen::Matrix<Scalar, 2, 1>;
template <typename Scalar>
using Mat2 = Eigen::Matrix<Scalar, 2, 2>;


// Rotates w/||w|| to u/||u|| by constructing the SO(2) rotation matrix
template <typename Scalar>
Mat2<Scalar> rotate(const Vec2<Scalar>& w, const Vec2<Scalar>& u)
{
    // Step 1: Normalize both w,u
    Vec2<Scalar> w_norm = w.normalized();
    Vec2<Scalar> u_norm = u.normalized();

    // Step 2: Rotation matrix J for R^2
    Mat2<Scalar> J;
    J << Scalar(0), Scalar(-1),
        Scalar(1), Scalar(0);

    // Step 3: Form W and U matrices
    Mat2<Scalar> W, U;
    W << w_norm, J * w_norm;
    U << u_norm, J * u_norm;

    // Step 4: Return rotation matrix
    return U * W.transpose();
}
