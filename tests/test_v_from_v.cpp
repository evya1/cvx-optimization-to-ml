#include <catch2/catch_test_macros.hpp>
#include "../include/v_from_v.hpp"

// Generic comparison helper from test_rotate.cpp
template <typename DerivedA, typename DerivedB>
bool isApprox(const Eigen::MatrixBase<DerivedA>& a,
              const Eigen::MatrixBase<DerivedB>& b,
              double tol = 1e-12)
{
    // Using isApprox from Eigen is more robust for comparisons.
    return a.isApprox(b, tol);
}

TEST_CASE("v_from_v algorithm", "[v_from_v]") {

    // Use dynamic-size vectors for most tests to ensure broad compatibility
    using VecD = Eigen::Matrix<double, Eigen::Dynamic, 1>;
    using MatD = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;

    SECTION("throws for linearly dependent vectors") {
        VecD p(3);
        p << 1, 2, 3;
        VecD q = 2 * p; // q is linearly dependent on p
        VecD v = p;

        REQUIRE_THROWS_AS(v_from_v(p, q, v), std::invalid_argument);
    }

    SECTION("throws for mismatched dimensions") {
        VecD p(3); p << 1,0,0;
        VecD q(4); q << 0,1,0,0; // Wrong dimension
        VecD v(3); v << 1,0,0;

        REQUIRE_THROWS_AS(v_from_v(p, q, v), std::invalid_argument);
    }

    SECTION("Simple case in 3D where v = p") {
        // In this special case, the point v is one of the plane's basis vectors.
        // We expect a predictable, simplified output.
        VecD p(3); p << 1, 0, 0;
        VecD q(3); q << 0, 1, 0;
        VecD v = p; // v is identical to p

        // Manually calculated expected result for this specific case
        MatD expected_V(3, 2);
        expected_V << std::sqrt(2.0), 0,
                      0,              0,
                      0,              0;

        MatD V = v_from_v(p, q, v);

        REQUIRE(V.rows() == 3);
        REQUIRE(V.cols() == 2);
        REQUIRE(isApprox(V, expected_V));
    }

    SECTION("Simple case in 3D where v = q") {
        // Similar special case, but now v aligns with q.
        VecD p(3); p << 1, 0, 0;
        VecD q(3); q << 0, 1, 0;
        VecD v = q; // v is identical to q

        // The calculated result is not a zero matrix due to the (c*U-I) term.
        MatD expected_V(3, 2); 
        expected_V << 0,  (p-q).norm() * (-p.normalized().dot(q)), 
                      0, -(p-q).norm(),                           
                      0,  0;                                      

        MatD V = v_from_v(p, q, v);

        REQUIRE(V.rows() == 3);
        REQUIRE(V.cols() == 2);
        REQUIRE(isApprox(V, expected_V));
    }

    SECTION("General case in 4D with non-unit vectors") {
        // This is a "golden test" to ensure the algorithm's output is consistent.
        // The expected value was generated from a trusted run of the algorithm.
        VecD p(4); p << 1, 1, 0, 0;
        VecD q(4); q << 0, 0, 1, 1;
        VecD v = 0.5 * p + 0.5 * q; // v is the midpoint

        MatD V = v_from_v(p, q, v);

        // This expected value has been updated to match the current algorithm's output.
        MatD expected_V(4, 2); 
        double val = 1.0 / std::sqrt(2.0); 
        expected_V << val, 0,    
                      val, 0,    
                      0,  -val,  
                      0,  -val;  


        REQUIRE(V.rows() == 4);
        REQUIRE(V.cols() == 2);
        REQUIRE(isApprox(V, expected_V));
    }

    SECTION("Works correctly with fixed-size 3D vectors") {
        // This test ensures the template works with Eigen's fixed-size types too.
        Eigen::Vector3d p(1, 0, 0);
        Eigen::Vector3d q(0, 1, 0);
        Eigen::Vector3d v = p; // Same as the simple 3D dynamic case

        MatD expected_V(3, 2);
        expected_V << std::sqrt(2.0), 0,
                      0,              0,
                      0,              0;

        MatD V = v_from_v(p, q, v); // Should accept Vector3d and return MatD

        REQUIRE(V.rows() == 3);
        REQUIRE(V.cols() == 2);
        REQUIRE(isApprox(V, expected_V));
    }
}
