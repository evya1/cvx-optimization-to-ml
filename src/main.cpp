#include <iostream>
#include "rotate.hpp"

int main() {
    Eigen::Vector2d w(1, 0);
    Eigen::Vector2d u(0, 1);

    auto R = rotate(w, u);
    std::cout << "Rotation matrix R:\n" << R << std::endl;
    std::cout << "R * w = " << (R * w).transpose() << std::endl;
    std::cout << "u = " << u.transpose() << std::endl;

    return 0;
}
