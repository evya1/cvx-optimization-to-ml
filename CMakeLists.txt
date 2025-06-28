# --- Basic Project Setup ---
cmake_minimum_required(VERSION 3.25)
project(cvx-optimization-to-ml)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find External Libraries ---
# Find Eigen before you define any targets that might use it.
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})

# --- Main Application Target ---
# 1. DEFINE the main executable with its source file(s).
add_executable(cvx_optimization_to_ml
        src/main.cpp
)

add_executable(test-runner
        tests/test_rotate.cpp
)
# 2. MODIFY the target to add its specific include directories.
# We make our project's "include" folder available to this target.
target_include_directories(cvx_optimization_to_ml PRIVATE include)


# --- Test Target ---
# 1. DEFINE the test executable with its source file(s).
#    A better name for a test executable is "test-runner" or similar.


# 2. MODIFY the test target with its dependencies.
#    The test code needs to see our project's headers.
target_include_directories(test-runner PRIVATE include)

#    The test code also needs Eigen. This one command adds both the
#    include directories for Eigen AND links the library.
target_link_libraries(test-runner PRIVATE Eigen3)