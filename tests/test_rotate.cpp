#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <Eigen/Dense>
#include "../include/rotate.hpp"

using Vec2d = Vec2<double>;
using Mat2d = Mat2<double>;

// Generic comparison for both vectors and matrices
template <typename DerivedA, typename DerivedB>
bool isApprox(const Eigen::MatrixBase<DerivedA>& a,
              const Eigen::MatrixBase<DerivedB>& b,
              double tol = 1e-12)
{
    return (a.derived() - b.derived()).norm() < tol;
}

TEST_CASE("rotate returns identity when vectors are equal") {
    Vec2d w(1, 0);
    Vec2d u(1, 0);

    Mat2d U = rotate(w, u);

    REQUIRE(isApprox(U, Mat2d::Identity()));
}

TEST_CASE("rotate maps x-axis to y-axis with 90-degree rotation") {
    Vec2d w(1, 0);
    Vec2d u(0, 1);

    Vec2d w_unit = w.normalized();
    Vec2d u_unit = u.normalized();

    Mat2d U = rotate(w_unit, u_unit);
    Vec2d rotated = U * w_unit;

    REQUIRE(isApprox(rotated, u_unit));
    REQUIRE(U.isUnitary(1e-12));
    REQUIRE(std::abs(U.determinant() - 1.0) < 1e-12);
}

TEST_CASE("rotate flips direction for opposite vectors (180-degree)") {
    Vec2d w(1, 0);
    Vec2d u(-1, 0);

    Vec2d w_unit = w.normalized();
    Vec2d u_unit = u.normalized();

    Mat2d U = rotate(w_unit, u_unit);
    Vec2d rotated = U * w_unit;

    REQUIRE(isApprox(rotated, u_unit));
    REQUIRE(std::abs(U.determinant() - 1.0) < 1e-12);
}

TEST_CASE("rotate aligns vectors with arbitrary angle") {
    double theta = M_PI / 4;
    Vec2d w(std::cos(theta), std::sin(theta));
    Vec2d u(-std::sin(theta), std::cos(theta));

    Vec2d w_unit = w.normalized();
    Vec2d u_unit = u.normalized();

    Mat2d U = rotate(w_unit, u_unit);
    Vec2d rotated = U * w_unit;

    REQUIRE(isApprox(rotated, u_unit));
    REQUIRE(U.isUnitary(1e-12));
}

TEST_CASE("rotate is invariant to scaling of input vectors") {
    Vec2d w(2, -1);
    Vec2d u(0, 2);

    Vec2d w_scaled = 5 * w;
    Vec2d u_scaled = -3 * u;

    Mat2d U1 = rotate(w, u);
    Mat2d U2 = rotate(w_scaled, u_scaled);

    bool equivalent = isApprox(U1, U2) || isApprox(U1, -U2);

    REQUIRE(equivalent);
    REQUIRE(U1.isUnitary(1e-12));
    REQUIRE(U2.isUnitary(1e-12));
}

TEST_CASE("aligning random vectors should pass") {
    for (int i = 0; i < 10; ++i) {
        Vec2d w = Vec2d::Random();
        Vec2d u = Vec2d::Random();

        if (w.norm() < 1e-10 || u.norm() < 1e-10)
            continue;

        Vec2d w_unit = w.normalized();
        Vec2d u_unit = u.normalized();
        Mat2d U = rotate(w_unit, u_unit);
        Vec2d rotated = U * w_unit;

        // Functional: rotation result aligns with target direction
        REQUIRE(isApprox(rotated, u_unit));

        // Structural: U is special orthogonal (SO(2))
        REQUIRE(U.isUnitary(1e-12));
        REQUIRE(std::abs(U.determinant() - 1.0) < 1e-12);
    }
}

TEST_CASE("rotate composition must be consistent: (1,0) → (0,1) → (-1,0)") {
    Vec2d w(1, 0);
    Vec2d u(0, 1);
    Vec2d v(-1, 0);

    Mat2d U1 = rotate(w, u);
    Mat2d U2 = rotate(u, v);
    Mat2d U3 = rotate(w, v);

    Mat2d composed = U2 * U1;

    REQUIRE(isApprox(composed, U3));
    REQUIRE(U1.isUnitary(1e-12));
    REQUIRE(U2.isUnitary(1e-12));
    REQUIRE(U3.isUnitary(1e-12));
}

TEST_CASE("rotate preserves norm of arbitrary vector") {
    Vec2d w(2, 3);
    Vec2d u(-1, 4);
    Vec2d v(5, -2);

    Mat2d U = rotate(w, u);
    Vec2d rotated_v = U * v;

    double original_norm = v.norm();
    double rotated_norm = rotated_v.norm();

    REQUIRE(std::abs(rotated_norm - original_norm) < 1e-12);
    REQUIRE(U.isUnitary(1e-12));
}

TEST_CASE("rotate throws on zero input vector") {
    Vec2d zero(0, 0);
    Vec2d unit(1, 0);

    REQUIRE_THROWS_AS(rotate(zero, unit), std::invalid_argument);
    REQUIRE_THROWS_AS(rotate(unit, zero), std::invalid_argument);
}

TEST_CASE("rotate maps (1,0) to (1,1)^T normalized") {
    Vec2d w(1, 0), u(3, 3);
    Vec2d w_unit = w.normalized();
    Vec2d u_unit = u.normalized();

    Mat2d U = rotate(w_unit, u_unit);
    Vec2d result = U * w_unit;

    REQUIRE(isApprox(result, u_unit));
    REQUIRE(U.isUnitary(1e-12));

    Mat2d expected_U;
    expected_U << 1, -1,
                  1,  1;
    expected_U.col(0).normalize();
    expected_U.col(1).normalize();

    REQUIRE(isApprox(U, expected_U));
}

TEST_CASE("rotate aligns random unit vectors by angle") {
    for (int i = 0; i < 10; ++i) {
        // Generate two random unit vectors
        Vec2d w = Vec2d::Random().normalized();
        Vec2d u = Vec2d::Random().normalized();

        // Skip near-zero vectors (extremely rare after normalization but still defensive)
        if (w.norm() < 1e-10 || u.norm() < 1e-10)
            continue;

        Mat2d U = rotate(w, u);
        Vec2d rotated = U * w;

        // Functional correctness: U * w ≈ u
        REQUIRE(isApprox(rotated, u));

        // Structural correctness: U ∈ SO(2)
        REQUIRE(U.isUnitary(1e-12));
        REQUIRE(std::abs(U.determinant() - 1.0) < 1e-12);
    }
}

TEST_CASE("Lemma 2: SO(2) is commutative — UR = RU for arbitrary angles") {
    // Any two 2×2 rotation matrices commute:
    // For R, U ∈ SO(2), we have UR = RU.

    using std::cos, std::sin;
    using Scalar = double;

    auto rotation = [](Scalar angle) -> Eigen::Matrix2d {
        return (Eigen::Matrix2d() <<
            cos(angle), -sin(angle),
            sin(angle),  cos(angle)).finished();
    };

    Scalar alpha = M_PI / 4;  // 45 degrees
    Scalar beta  = M_PI / 3;  // 60 degrees

    Eigen::Matrix2d R = rotation(alpha);
    Eigen::Matrix2d U = rotation(beta);

    Eigen::Matrix2d UR = U * R;
    Eigen::Matrix2d RU = R * U;

    REQUIRE((UR - RU).norm() < 1e-12);
}
