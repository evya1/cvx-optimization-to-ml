#include "rotate.hpp"


Eigen::Vector2d rotateVector(const Eigen::Vector2d& v, double angle_radians) {
    Eigen::Rotation2Dd rotation(angle_radians);
    return rotation * v;
}

