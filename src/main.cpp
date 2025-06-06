#include <iostream>
// include <Eigen/Dense>
#include "rotate.hpp"

int main() {
    using Eigen::MatrixXd;
    using Eigen::VectorXd;
    using std::cout;
    using std::endl;
    VectorXd v(2);
    v << 1, 0;
    std::cout << "v = \n" << v[1] << std::endl;

    const VectorXd rotated = rotateVector(v, M_PI_4);

    cout << "Original vector: " << v.transpose() << endl;
    cout << "Rotated vector:  " << rotated.transpose() << endl;

    MatrixXd m(2,2);
    m(0,0) = 3;
    m(1,0) = 2.5;
    m(0,1) = -1;
    m(1,1) = m(1,0) + m(0,1);
    cout << m << endl;

    MatrixXd A = MatrixXd::Zero(3, 3);
    cout << A << endl;

    return 0;
}
