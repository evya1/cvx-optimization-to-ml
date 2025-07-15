#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include "../include/rotate.hpp"
#include <type_traits>

/**
 * @brief A more robust, type-aware comparison helper for Eigen types.
 *
 * This version uses Eigen's built-in isApprox() and automatically adjusts
 * the tolerance based on whether the scalar type is a float or a double.
 */
template <typename DerivedA, typename DerivedB>
bool isApprox(const Eigen::MatrixBase<DerivedA>& a,
              const Eigen::MatrixBase<DerivedB>& b)
{
    using Scalar = typename DerivedA::Scalar;
    // Use a looser tolerance for single-precision floats
    const auto tolerance = std::is_same_v<Scalar, float> ? 1e-6 : 1e-12;
    return a.isApprox(b, static_cast<Scalar>(tolerance));
}

/**
 * @brief This single test case is instantiated for each type in the list (float, double).
 * All existing SECTIONs are now run for each scalar type, ensuring broad coverage.
 */
TEMPLATE_TEST_CASE("Rotate function tests for multiple types", "[rotate][types]", float, double)
{
    // Define type aliases based on the current TestType (float or double)
    using Scalar = TestType;
    using Vec2 = Eigen::Matrix<Scalar, 2, 1>;
    using Mat2 = Eigen::Matrix<Scalar, 2, 2>;
    const Scalar tol = std::is_same_v<Scalar, float> ? 1e-6 : 1e-12;

    SECTION("returns identity when vectors are equal")
    {
        Vec2 w(1, 0);
        Vec2 u(1, 0);
        Mat2 U = rotate(w, u);
        REQUIRE(isApprox(U, Mat2::Identity()));
    }

    SECTION("maps x-axis to y-axis with 90-degree rotation")
    {
        Vec2 w(1, 0);
        Vec2 u(0, 1);
        Vec2 w_unit = w.normalized();
        Vec2 u_unit = u.normalized();

        Mat2 U = rotate(w_unit, u_unit);
        Vec2 rotated = U * w_unit;

        REQUIRE(isApprox(rotated, u_unit));
        REQUIRE(U.isUnitary(tol));
        REQUIRE(std::abs(U.determinant() - static_cast<Scalar>(1.0)) < tol);
    }

    SECTION("flips direction for opposite vectors (180-degree)")
    {
        Vec2 w(1, 0);
        Vec2 u(-1, 0);
        Vec2 w_unit = w.normalized();
        Vec2 u_unit = u.normalized();

        Mat2 U = rotate(w_unit, u_unit);
        Vec2 rotated = U * w_unit;

        REQUIRE(isApprox(rotated, u_unit));
        REQUIRE(std::abs(U.determinant() - static_cast<Scalar>(1.0)) < tol);
    }

    SECTION("aligns vectors with arbitrary angle")
    {
        Scalar theta = M_PI / 4.0;
        Vec2 w(std::cos(theta), std::sin(theta));
        Vec2 u(-std::sin(theta), std::cos(theta));

        Vec2 w_unit = w.normalized();
        Vec2 u_unit = u.normalized();

        Mat2 U = rotate(w_unit, u_unit);
        Vec2 rotated = U * w_unit;

        REQUIRE(isApprox(rotated, u_unit));
        REQUIRE(U.isUnitary(tol));
    }

    SECTION("is invariant to positive scaling of input vectors")
    {
        Vec2 w(2, -1);
        Vec2 u(0, 2);

        Vec2 w_scaled = static_cast<Scalar>(5) * w;
        Vec2 u_scaled = static_cast<Scalar>(3) * u;

        Mat2 U1 = rotate(w, u);
        Mat2 U2 = rotate(w_scaled, u_scaled);

        REQUIRE(isApprox(U1, U2));
        REQUIRE(U1.isUnitary(tol));
        REQUIRE(U2.isUnitary(tol));
    }

    SECTION("aligning random vectors should pass")
    {
        for (int i = 0; i < 10; ++i)
        {
            Vec2 w = Vec2::Random();
            Vec2 u = Vec2::Random();

            if (w.norm() < static_cast<Scalar>(1e-10) || u.norm() < static_cast<Scalar>(1e-10))
                continue;

            Vec2 w_unit = w.normalized();
            Vec2 u_unit = u.normalized();
            Mat2 U = rotate(w_unit, u_unit);
            Vec2 rotated = U * w_unit;

            REQUIRE(isApprox(rotated, u_unit));
            REQUIRE(U.isUnitary(tol));
            REQUIRE(std::abs(U.determinant() - static_cast<Scalar>(1.0)) < tol);
        }
    }

    SECTION("composition must be consistent: (1,0) -> (0,1) -> (-1,0)")
    {
        Vec2 w(1, 0);
        Vec2 u(0, 1);
        Vec2 v(-1, 0);

        Mat2 U1 = rotate(w, u);
        Mat2 U2 = rotate(u, v);
        Mat2 U3 = rotate(w, v);

        Mat2 composed = U2 * U1;

        REQUIRE(isApprox(composed, U3));
        REQUIRE(U1.isUnitary(tol));
        REQUIRE(U2.isUnitary(tol));
        REQUIRE(U3.isUnitary(tol));
    }

    SECTION("preserves norm of arbitrary vector")
    {
        Vec2 w(2, 3);
        Vec2 u(-1, 4);
        Vec2 v(5, -2);

        Mat2 U = rotate(w, u);
        Vec2 rotated_v = U * v;

        Scalar original_norm = v.norm();
        Scalar rotated_norm = rotated_v.norm();

        REQUIRE(std::abs(rotated_norm - original_norm) < tol);
        REQUIRE(U.isUnitary(tol));
    }

    SECTION("throws on zero input vector")
    {
        Vec2 zero(0, 0);
        Vec2 unit(1, 0);

        REQUIRE_THROWS_AS(rotate(zero, unit), std::invalid_argument);
        REQUIRE_THROWS_AS(rotate(unit, zero), std::invalid_argument);
    }

    // --- FIX: The following 3 sections were updated to be type-generic ---

    SECTION("maps (1,0) to (1,1)^T normalized")
    {
        Vec2 w(1, 0), u(3, 3);
        Vec2 w_unit = w.normalized();
        Vec2 u_unit = u.normalized();

        Mat2 U = rotate(w_unit, u_unit);
        Vec2 result = U * w_unit;

        REQUIRE(isApprox(result, u_unit));
        REQUIRE(U.isUnitary(tol));

        Mat2 expected_U;
        expected_U << 1, -1,
            1, 1;

        // This is a rotation matrix, so columns are orthonormal
        expected_U.col(0).normalize();
        expected_U.col(1) = Vec2(-expected_U(1,0), expected_U(0,0)); // Ensure it's in SO(2)

        REQUIRE(isApprox(U, expected_U));
    }

    SECTION("aligns random unit vectors by angle")
    {
        for (int i = 0; i < 10; ++i)
        {
            Vec2 w = Vec2::Random().normalized();
            Vec2 u = Vec2::Random().normalized();

            if (w.norm() < static_cast<Scalar>(1e-10) || u.norm() < static_cast<Scalar>(1e-10))
                continue;

            Mat2 U = rotate(w, u);
            Vec2 rotated = U * w;

            REQUIRE(isApprox(rotated, u));
            REQUIRE(U.isUnitary(tol));
            REQUIRE(std::abs(U.determinant() - static_cast<Scalar>(1.0)) < tol);
        }
    }

    SECTION("Lemma 2: SO(2) is commutative — UR = RU for arbitrary angles")
    {
        using std::cos, std::sin;

        auto rotation = [](Scalar angle) -> Mat2
        {
            return (Mat2() <<
                cos(angle), -sin(angle),
                sin(angle),  cos(angle)).finished();
        };

        Scalar alpha = M_PI / 4.0;
        Scalar beta  = M_PI / 3.0;

        Mat2 R = rotation(alpha);
        Mat2 U = rotation(beta);

        Mat2 UR = U * R;
        Mat2 RU = R * U;

        REQUIRE(isApprox(UR, RU));
    }
}
