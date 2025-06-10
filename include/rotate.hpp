#pragma once

#include <Eigen/Dense>

// Rotates w/||w|| to u/||u|| by constructing the SO(2) rotation matrix
template<typename Scalar>
Eigen::Matrix<Scalar, 2, 2>
rotate(const Eigen::Matrix<Scalar, 2, 1>& w,
       const Eigen::Matrix<Scalar, 2, 1>& u)
{
    // Step 1: Normalize both input vectors
    Eigen::Matrix<Scalar, 2, 1> w_norm = w.normalized();
    Eigen::Matrix<Scalar, 2, 1> u_norm = u.normalized();

    // Step 2: Rotation matrix J for R^2
    Eigen::Matrix<Scalar, 2, 2> J;
    J << Scalar(0), Scalar(-1),
         Scalar(1), Scalar(0);

    // Step 3: Form W and U matrices
    Eigen::Matrix<Scalar, 2, 2> W, U;
    W << w_norm, J * w_norm;
    U << u_norm, J * u_norm;

    // Step 4: Return rotation matrix
    return U * W.transpose();
}
