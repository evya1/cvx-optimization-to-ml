#include <iostream>
#include <Eigen/Dense>
#include <cassert>
#include <cmath>
#include "rotate.hpp"

using Vec2d = Vec2<double>;
using Mat2d = Mat2<double>;

bool isApprox(const Eigen::Vector2d& a, const Eigen::Vector2d& b, double tol = 1e-12)
{
    return (a - b).norm() < tol;
}

bool isApprox(const Eigen::Matrix2d& a, const Eigen::Matrix2d& b, double tol = 1e-12)
{
    return (a - b).norm() < tol;
}

void test_rotate_identical_vectors_should_return_identity()
{
    Vec2d w(1, 0), u(1, 0);
    Mat2d U = rotate(w, u);
    assert(isApprox(U, Mat2d::Identity()));
    std::cout << __func__ << ": passed\n";
}

void test_rotate_axis_aligned_should_return_90_deg_rotation()
{
    Vec2d w(1, 0), u(0, 1);
    Mat2d U = rotate(w, u);
    Vec2d res = U * w.normalized();
    assert(isApprox(res, u.normalized()));
    assert(std::abs(U.determinant() - 1.0) < 1e-12);
    auto ortho = U.transpose() * U;
    assert(isApprox(Mat2d(ortho), Mat2d::Identity()));
    std::cout << __func__ << ": passed\n";
}

void test_rotate_opposite_vectors_should_return_180_deg_rotation()
{
    Vec2d w(1, 0), u(-1, 0);
    Mat2d U = rotate(w, u);
    Vec2d res = U * w.normalized();
    assert(isApprox(res, u.normalized()));
    assert(std::abs(U.determinant() - 1.0) < 1e-12);
    std::cout << __func__ << ": passed\n";
}

void test_rotate_arbitrary_angle_should_map_direction_correctly()
{
    double theta = M_PI / 4;
    Vec2d w(std::cos(theta), std::sin(theta));
    Vec2d u(-std::sin(theta), std::cos(theta));
    Mat2d U = rotate(w, u);
    Vec2d res = U * w.normalized();
    assert(isApprox(res, u.normalized()));
    std::cout << __func__ << ": passed\n";
}

void test_rotate_scaled_vectors_should_give_same_result()
{
    Vec2d w(2, -1), u(0, 2);
    Vec2d w_scaled = 5 * w;
    Vec2d u_scaled = -3 * u;
    Mat2d U1 = rotate(w, u);
    Mat2d U2 = rotate(w_scaled, u_scaled);
    assert(isApprox(U1, U2) || isApprox(U1, -U2)); // Up to sign if both flipped
    std::cout << __func__ << ": passed\n";
}

void test_rotate_random_vectors_should_satisfy_so2_properties()
{
    for (int i = 0; i < 10; ++i)
    {
        Vec2d w = Eigen::Vector2d::Random();
        Vec2d u = Eigen::Vector2d::Random();
        if (w.norm() < 1e-10 || u.norm() < 1e-10) continue; // skip degenerate
        Mat2d U = rotate(w, u);
        assert(isApprox(U * w.normalized(), u.normalized()));
        assert(std::abs(U.determinant() - 1.0) < 1e-12);
        auto ortho = U.transpose() * U;
        assert(isApprox(Mat2d(ortho), Mat2d::Identity()));
    }
    std::cout << __func__ << ": passed\n";
}

void test_rotate_composition_should_be_consistent()
{
    Vec2d w(1, 0), u(0, 1), v(-1, 0);
    Mat2d U1 = rotate(w, u);
    Mat2d U2 = rotate(u, v);
    Mat2d U3 = rotate(w, v);
    assert(isApprox(U2 * U1, U3));
    std::cout << __func__ << ": passed\n";
}

void test_rotate_length_preservation()
{
    Vec2d w(2, 3), u(-1, 4);
    Mat2d U = rotate(w, u);
    Vec2d v(5, -2);
    assert(std::abs((U * v).norm() - v.norm()) < 1e-12);
    std::cout << __func__ << ": passed\n";
}

void test_rotate_should_throw_on_zero_vector_input()
{
    Vec2d zero(0, 0), u(1, 0), w(1, 0);
    bool caught = false;
    try { rotate(zero, u); }
    catch (const std::invalid_argument&) { caught = true; }
    assert(caught);

    caught = false;
    try { rotate(w, zero); }
    catch (const std::invalid_argument&) { caught = true; }
    assert(caught);
    std::cout << __func__ << ": passed\n";
}

int main()
{
    test_rotate_identical_vectors_should_return_identity();
    test_rotate_axis_aligned_should_return_90_deg_rotation();
    test_rotate_opposite_vectors_should_return_180_deg_rotation();
    test_rotate_arbitrary_angle_should_map_direction_correctly();
    test_rotate_scaled_vectors_should_give_same_result();
    test_rotate_random_vectors_should_satisfy_so2_properties();
    test_rotate_composition_should_be_consistent();
    test_rotate_length_preservation();
    test_rotate_should_throw_on_zero_vector_input();

    std::cout << "\nAll rotate() tests passed!\n";
    return 0;
}
