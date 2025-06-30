cmake_minimum_required(VERSION 3.22)
project(cvx-optimization-to-ml CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# === Fetch Dependencies ===
include(FetchContent)

# Eigen (header-only)
FetchContent_Declare(
        eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4.0
)
FetchContent_MakeAvailable(eigen)
include_directories(${eigen_SOURCE_DIR})

# Catch2 (for testing)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)

# nlohmann_json (header-only, for C++ -> JSON exporting)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# === Project Structure ===
include_directories(include)

# Main executable (demonstrator)
add_executable(cvx-optimization-to-ml src/main.cpp)
# <<< FIX: Link main executable to Eigen >>>
target_link_libraries(cvx-optimization-to-ml PRIVATE Eigen3::Eigen)

# Exporter executable (C++ -> JSON)
add_executable(fg_to_json src/fg_to_json.cpp)
# <<< FIX: Link fg_to_json to Eigen and nlohmann_json >>>
target_link_libraries(fg_to_json PRIVATE nlohmann_json::nlohmann_json Eigen3::Eigen)

# === Testing Setup ===
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(CTest)
enable_testing()

# Test executable
add_executable(test-runner tests/test_rotate.cpp tests/test_v_from_v.cpp tests/test_types_v_from_v.cpp)
# <<< FIX: Link test runner to Catch2 and Eigen >>>
target_link_libraries(test-runner PRIVATE Catch2::Catch2WithMain Eigen3::Eigen)

# Define a test for CTest
add_test(NAME unit-tests COMMAND test-runner)