#include <iostream>
#include <Eigen/Dense>
#include "rotate.hpp"
#include "v_from_v.hpp"
#include "pqbu.hpp"
#include "fg.hpp"

#include <sstream>

void readVector2(Eigen::Vector2d& v, const std::string& name) {
    std::cout << "Enter 2D vector " << name << " (format: x y): ";
    std::string line;
    std::getline(std::cin >> std::ws, line);  // std::ws to skip leading whitespace

    std::istringstream iss(line);
    double x, y;
    iss >> x >> y;

    v = Eigen::Vector2d(x, y);

    double extra;
    if (iss >> extra) {
        std::cerr << "Warning: Too many values entered for " << name << ". Extra values will be ignored.\n";
    }
}

void readVector3(Eigen::Vector3d& v, const std::string& name) {
    std::cout << "Enter 3D vector " << name << " (format: x y z): ";
    std::string line;
    std::getline(std::cin >> std::ws, line);

    std::istringstream iss(line);
    double x, y, z;
    iss >> x >> y >> z;

    v = Eigen::Vector3d(x, y, z);

    double extra;
    if (iss >> extra) {
        std::cerr << "Warning: Too many values entered for " << name << ". Extra values will be ignored.\n";
    }
}


int main() {
    std::cout << "Choose algorithms to run (e.g., 1 3 4):\n";
    std::cout << "1 - Rotate(w, u)\n";
    std::cout << "2 - V-from-v(p, q, v)\n";
    std::cout << "3 - PQBU(p, q, z)\n";
    std::cout << "4 - FG(p, q, z, ℓ)\n";

    std::vector<int> choices;
    int choice;
    while (std::cin >> choice) {
        if (choice < 1 || choice > 4) break;
        choices.push_back(choice);
        if (std::cin.peek() == '\n') break;
    }

    for (int algo : choices) {
        switch (algo) {
            case 1: {
                std::cout << "\n=== Algorithm 1: Rotate(w, u) ===\n";
                Eigen::Vector2d w, u;
                readVector2(w, "w");
                readVector2(u, "u");
                auto R = rotate(w, u);
                std::cout << "Rotation matrix R:\n" << R << std::endl;
                std::cout << "R * w = " << (R * w).transpose() << " (should be close to u)\n";
                std::cout << "u = " << u.transpose() << "\n";
                break;
            }
            case 2: {
                std::cout << "\n=== Algorithm 2: V-from-v(p, q, v) ===\n";
                Eigen::Vector3d p, q, v;
                readVector3(p, "p");
                readVector3(q, "q");
                readVector3(v, "v");
                auto V = v_from_v(p, q, v);
                std::cout << "V(v) matrix:\n" << V << "\n";
                break;
            }
            case 3: {
                std::cout << "\n=== Algorithm 3: PQBU(p, q, z) ===\n";
                Eigen::Vector3d p, q, z;
                readVector3(p, "p");
                readVector3(q, "q");
                readVector3(z, "z");

                if (p.isZero()) {
                    std::cerr << "Error: Vector p is zero.\n";
                    break;
                }
                if (p.cross(q).isZero()) {
                    std::cerr << "Error: Vectors p and q are linearly dependent.\n";
                    break;
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
                }
                break;
            }
            case 4: {
                std::cout << "\n=== Algorithm 4: FG(p, q, z, ℓ) ===\n";
                Eigen::Vector3d p, q, z, ell;
                readVector3(p, "p");
                readVector3(q, "q");
                readVector3(z, "z");
                readVector3(ell, "ℓ");

                try {
                    auto [F, G, c] = fg(p, q, z, ell);
                    std::cout << "F matrix:\n" << F << "\n\n";
                    std::cout << "G matrix:\n" << G << "\n\n";
                    std::cout << "c scalar: " << c << "\n\n";
                } catch (const std::exception& e) {
                    std::cerr << "FG failed: " << e.what() << "\n";
                }
                break;
            }
        }
    }

    return 0;
}