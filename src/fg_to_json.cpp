#include <iostream>
#include <fstream>
#include <vector>
#include <Eigen/Dense>
#include <nlohmann/json.hpp>
#include "fg.hpp"

using json = nlohmann::json;

/**
 * @brief Converts an Eigen::Matrix to a std::vector of std::vectors.
 * This is a helper function to make serialization to JSON straightforward.
 */
template<typename T>
std::vector<std::vector<T>> matrix_to_vector(const Eigen::Matrix<T, 2, 2>& mat) {
    std::vector<std::vector<T>> vec(2, std::vector<T>(2));
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            vec[i][j] = mat(i, j);
        }
    }
    return vec;
}

int main() {

    // 1. Define the input vectors for the problem.
    Eigen::Vector3d p(1.0, 0.0, 0.0);
    Eigen::Vector3d q(0.0, 1.0, 0.0);
    Eigen::Vector3d z(1.0, 1.0, 1.0);
    Eigen::Vector3d ell(0.0, 0.0, 1.0);

    // 2. Run Algorithm 4 to get F, G, and c.
    // The path is relative to the build directory where the executable runs.
    std::cout << "[C++] Running fg() to compute parameters F, G, c..." << std::endl;
    auto [F, G, c] = fg(p, q, z, ell);

    // 3. Create a JSON object.
    json output_json;
    output_json["F"] = matrix_to_vector(F);
    output_json["G"] = matrix_to_vector(G);
    output_json["c"] = c;

    // 4. Write the JSON object to the file.
    const std::string output_path = "../inputs/eq172_params.json";
    std::ofstream output_file(output_path);
    if (!output_file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << output_path << std::endl;
        return 1;
    }
    output_file << output_json.dump(4);
    output_file.close();

    std::cout << "[C++] Successfully wrote parameters to " << output_path << std::endl;

    return 0;
}