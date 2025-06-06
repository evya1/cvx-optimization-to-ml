cmake_minimum_required(VERSION 3.25)
project(cvx-optimization-to-ml)

set(CMAKE_CXX_STANDARD 17)

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})

add_executable(cvx-optimization-to-ml
        src/main.cpp
        src/rotate.cpp
)

target_include_directories(cvx-optimization-to-ml PRIVATE include)
