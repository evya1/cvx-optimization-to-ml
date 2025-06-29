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

TEST_CASE("rotate() returns identity when input vectors are identical") {
    Vec2d w(1, 0), u(1, 0);
    Mat2d U = rotate(w, u);
    REQUIRE(isApprox(U, Mat2d::Identity()));
}

TEST_CASE("rotate() returns 90-degree rotation when vectors are axis-aligned") {
    Vec2d w(1, 0), u(0, 1);
    Mat2d U = rotate(w, u);
    Vec2d res = U * w.normalized();
    REQUIRE(isApprox(res, u.normalized()));
    REQUIRE(std::abs(U.determinant() - 1.0) < 1e-12);
    REQUIRE(isApprox((U.transpose() * U).eval(), Mat2d::Identity()));
}

TEST_CASE("rotate() returns 180-degree rotation for opposite vectors") {
    Vec2d w(1, 0), u(-1, 0);
    Mat2d U = rotate(w, u);
    REQUIRE(isApprox(U * w.normalized(), u.normalized()));
    REQUIRE(std::abs(U.determinant() - 1.0) < 1e-12);
}

TEST_CASE("rotate() maps arbitrary angle correctly") {
    double theta = M_PI / 4;
    Vec2d w(std::cos(theta), std::sin(theta));
    Vec2d u(-std::sin(theta), std::cos(theta));
    Mat2d U = rotate(w, u);
    REQUIRE(isApprox(U * w.normalized(), u.normalized()));
}

TEST_CASE("rotate() is scale-invariant for both vectors") {
    Vec2d w(2, -1), u(0, 2);
    Vec2d w_scaled = 5 * w;
    Vec2d u_scaled = -3 * u;
    Mat2d U1 = rotate(w, u);
    Mat2d U2 = rotate(w_scaled, u_scaled);
    REQUIRE((isApprox(U1, U2) || isApprox(U1, -U2)));
}

TEST_CASE("rotate() satisfies SO(2) properties for random vectors") {
    for (int i = 0; i < 10; ++i) {
        Vec2d w = Eigen::Vector2d::Random();
        Vec2d u = Eigen::Vector2d::Random();
        if (w.norm() < 1e-10 || u.norm() < 1e-10) continue;
        Mat2d U = rotate(w, u);
        REQUIRE(isApprox(U * w.normalized(), u.normalized()));
        REQUIRE(std::abs(U.determinant() - 1.0) < 1e-12);
        REQUIRE(isApprox((U.transpose() * U).eval(), Mat2d::Identity()));
    }
}

TEST_CASE("rotate() composition is consistent") {
    Vec2d w(1, 0), u(0, 1), v(-1, 0);
    Mat2d U1 = rotate(w, u);
    Mat2d U2 = rotate(u, v);
    Mat2d U3 = rotate(w, v);
    REQUIRE(isApprox(U2 * U1, U3));
}

TEST_CASE("rotate() preserves vector length") {
    Vec2d w(2, 3), u(-1, 4);
    Mat2d U = rotate(w, u);
    Vec2d v(5, -2);
    REQUIRE(std::abs((U * v).norm() - v.norm()) < 1e-12);
}

TEST_CASE("rotate() throws on zero vector input") {
    Vec2d zero(0, 0), u(1, 0), w(1, 0);
    REQUIRE_THROWS_AS(rotate(zero, u), std::invalid_argument);
    REQUIRE_THROWS_AS(rotate(w, zero), std::invalid_argument);
}
