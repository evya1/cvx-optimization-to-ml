#include <iostream>
#include <Eigen/Dense>
#include "rotate.hpp"
#include "v_from_v.hpp"
#include "pqbu.hpp"
#include "fg.hpp"

int main() {
    std::cout << "=== Algorithm 1: Rotate(w, u) ===\n";
    Eigen::Vector2d w(1, 0);
    Eigen::Vector2d u(0, 1);

    auto R = rotate(w, u);
    std::cout << "Rotation matrix R:\n" << R << std::endl;
    std::cout << "R * w = " << (R * w).transpose() << " (should be close to u)\n";
    std::cout << "u = " << u.transpose() << "\n\n";


    std::cout << "=== Algorithm 2: V-from-v(p, q, v) ===\n";
    Eigen::Vector3d p(1, 0, 0);
    Eigen::Vector3d q(0, 1, 0);
    Eigen::Vector3d v = (p + q) / 2.0;

    auto V = v_from_v(p, q, v);
    std::cout << "V(v) matrix:\n" << V << "\n\n";


    // Example usage of Algorithm 3 (PQBU) with basic test inputs
    std::cout << "=== Algorithm 3: PQBU(p, q, z) ===\n";
    // Input for testing Algorithm 3
    Eigen::Vector3d z(0.5, 0.5, 1.0);

    // Check that p ≠ 0
    if (p.isZero()) {
        std::cerr << "Error: Vector p is zero.\n";
        return 1;
    }

    // Check that q ∉ span{p} by verifying p × q ≠ 0
    if (p.cross(q).isZero()) {
        std::cerr << "Error: Vectors p and q are linearly dependent.\n";
        return 1;
    }

    try {
        auto [P, Q, B, U, u] = pqbu(p, q, z);
        std::cout << "P matrix:\n" << P << "\n\n";
        std::cout << "Q matrix:\n" << Q << "\n\n";
        std::cout << "B matrix:\n" << B << "\n\n";
        std::cout << "U matrix:\n" << U << "\n\n";
        std::cout << "u vector: " << u.transpose() << "\n\n";
    } catch (const std::exception& e) {
        std::cerr << "PQBU failed: " << e.what() << "\n";
        return 1;
    }

    std::cout << "=== Algorithm 4: FG(p, q, z, ℓ) ===\n";
    Eigen::Vector3d ell(0, 0, 1);  // ℓ defines a line in z-direction

    try {
        auto [F, G, c] = fg(p, q, z, ell);
        std::cout << "F matrix:\n" << F << "\n\n";
        std::cout << "G matrix:\n" << G << "\n\n";
        std::cout << "c scalar: " << c << "\n\n";
    } catch (const std::exception& e) {
        std::cerr << "FG failed: " << e.what() << "\n";
        return 1;
    }

    // Run FG (Algorithm 4) with valid mock input vectors
    std::cout << "=== Algorithm 4: FG Mock Test ===\n";

    // Define inputs
    Eigen::Vector3d p2(1, 0, 0);
    Eigen::Vector3d q2(0, 1, 0);
    Eigen::Vector3d z2(1, 1, 1);
    Eigen::Vector3d ell2(0, 0, 1);

    // Run FG using mock-valid inputs that satisfy the mathematical requirements
    try {
        auto [F2, G2, c2] = fg(p2, q2, z2, ell2);
        std::cout << "F matrix (mock test):\n" << F2 << "\n\n";
        std::cout << "G matrix (mock test):\n" << G2 << "\n\n";
        std::cout << "c scalar (mock test): " << c2 << "\n\n";
    } catch (const std::exception& e) {
        std::cerr << "Mock FG run failed: " << e.what() << "\n";
        return 1;
    }


    return 0;
}