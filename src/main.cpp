#include <iostream>
#include "rotate.hpp"
#include "v_from_v.hpp"

int main() {
    Eigen::Vector2d w(1, 0);
    Eigen::Vector2d u(0, 1);

    auto R = rotate(w, u);
    std::cout << "Rotation matrix R:\n" << R << std::endl;
    std::cout << "R * w = " << (R * w).transpose() << std::endl;
    std::cout << "u = " << u.transpose() << std::endl;

    // Use dynamic-sized vectors
    Eigen::VectorXd p(3), q(3), v(3);
    p << 1, 0, 0;
    q << 0, 1, 0;
    v = (p + q) / 2.0;

    auto V = v_from_v(p, q, v);
    std::cout << "\nV(v) matrix from Algorithm 2:\n" << V << std::endl;

    return 0;
}