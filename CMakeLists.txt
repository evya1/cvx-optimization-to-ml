cmake_minimum_required(VERSION 3.25)
project(cvx-optimization-to-ml)

set(CMAKE_CXX_STANDARD 17)

# --- Find Eigen ---
find_package(Eigen3 REQUIRED HINTS /opt/homebrew/share/eigen3 /opt/homebrew/include/eigen3)

if (TARGET Eigen3::Eigen)
    get_target_property(EIGEN3_INCLUDE_DIR Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "Found Eigen3 include dir: ${EIGEN3_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "Could not find Eigen3::Eigen target")
endif()


# --- Main Executable ---
add_executable(cvx-optimization-to-ml src/main.cpp)
target_include_directories(cvx-optimization-to-ml PRIVATE
        ${Eigen3_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(cvx-optimization-to-ml PRIVATE Eigen3::Eigen)

message(STATUS "Root: ${PROJECT_SOURCE_DIR}")

# === Testing Setup ===
enable_testing()

# --- Automatically find all .cpp files in tests/ ---
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/tests/*.cpp)

# --- Combine them into a single test-runner executable ---
add_executable(test-runner ${TEST_SOURCES})
target_include_directories(test-runner PRIVATE
        ${Eigen3_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(test-runner PRIVATE Eigen3::Eigen)

# --- Register test-runner with CTest ---
add_test(NAME all-tests COMMAND test-runner)