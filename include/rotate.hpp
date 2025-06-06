#pragma once

#include <Eigen/Dense>
using Eigen::MatrixXd;

// Rotates a 2D vector v by angle (in radians) using Eigen::Rotation2Dd
Eigen::Vector2d rotateVector(const Eigen::Vector2d& v, double angle_radians);
