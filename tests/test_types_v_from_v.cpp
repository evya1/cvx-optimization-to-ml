#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include "../include/v_from_v.hpp"
#include <type_traits>

/**
 * @brief Generic comparison helper that adapts tolerance to scalar type.
 *
 * For single-precision floats, a larger tolerance is used.
 */
template <typename DerivedA, typename DerivedB>
bool isApprox(const Eigen::MatrixBase<DerivedA>& a,
              const Eigen::MatrixBase<DerivedB>& b)
{
    using Scalar = typename DerivedA::Scalar;
    auto tolerance = std::is_same_v<Scalar, float> ? 1e-6 : 1e-12;
    return a.isApprox(b, static_cast<Scalar>(tolerance));
}

/**
 * @brief Template test case for v_from_v.
 *
 * This test block will be compiled and run twice: once with TestType=float
 * and once with TestType=double. This verifies that the function works
 * correctly for different floating-point precisions.
 */
TEMPLATE_TEST_CASE("v_from_v type compatibility tests", "[v_from_v][types]", float, double) {
    // TestType will be 'float' or 'double' depending on the run.
    using Scalar = TestType;

    // Define Eigen types based on the current TestType
    using Vec3 = Eigen::Matrix<Scalar, 3, 1>;
    using VecD = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;
    using MatD = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;

    SECTION("Function returns correct type and value for dynamic-size vectors") {
        VecD p(3);
        p << static_cast<Scalar>(1), static_cast<Scalar>(0), static_cast<Scalar>(0);
        VecD q(3);
        q << static_cast<Scalar>(0), static_cast<Scalar>(1), static_cast<Scalar>(0);
        VecD v = p;

        // The 'auto' keyword deduces the return type.
        auto V = v_from_v(p, q, v);

        // 1. Static (compile-time) check of the return type.
        // This line verifies that the output matrix has the same scalar type (float/double)
        // as the input vectors. This is a core part of type testing.
        STATIC_REQUIRE(std::is_same_v<typename decltype(V)::Scalar, Scalar>);

        // 2. Runtime check for numerical correctness.
        MatD expected_V(3, 2);
        expected_V << static_cast<Scalar>(std::sqrt(2.0)), static_cast<Scalar>(0),
                      static_cast<Scalar>(0),               static_cast<Scalar>(0),
                      static_cast<Scalar>(0),               static_cast<Scalar>(0);

        REQUIRE(V.rows() == 3);
        REQUIRE(V.cols() == 2);
        REQUIRE(isApprox(V, expected_V));
    }

    SECTION("Function returns correct type and value for fixed-size vectors") {
        // Here, we provide fixed-size (compile-time) inputs.
        Vec3 p(static_cast<Scalar>(1), static_cast<Scalar>(0), static_cast<Scalar>(0));
        Vec3 q(static_cast<Scalar>(0), static_cast<Scalar>(1), static_cast<Scalar>(0));
        Vec3 v = p;

        auto V = v_from_v(p, q, v);

        // 1. Static check to ensure template deduction is correct.
        STATIC_REQUIRE(std::is_same_v<typename decltype(V)::Scalar, Scalar>);

        // 2. Static check to confirm the return type is a dynamic matrix.
        // Your function signature specifies a return type of ::MatD<Scalar>, which is dynamic.
        // This confirms it, even when the input is fixed-size.
        STATIC_REQUIRE(decltype(V)::RowsAtCompileTime == Eigen::Dynamic);
        STATIC_REQUIRE(decltype(V)::ColsAtCompileTime == Eigen::Dynamic);

        // 3. Runtime check for numerical correctness.
        MatD expected_V(3, 2);
        expected_V << static_cast<Scalar>(std::sqrt(2.0)), static_cast<Scalar>(0),
                      static_cast<Scalar>(0),               static_cast<Scalar>(0),
                      static_cast<Scalar>(0),               static_cast<Scalar>(0);

        REQUIRE(isApprox(V, expected_V));
    }
}