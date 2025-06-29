cmake_minimum_required(VERSION 3.25)
project(cvx-optimization-to-ml)

set(CMAKE_CXX_STANDARD 17)

# --- Diagnostic Info (Optional but helpful) ---
message(STATUS "Using CMake generator: ${CMAKE_GENERATOR}")
message(STATUS "System name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Project root: ${PROJECT_SOURCE_DIR}")

# --- Find Eigen ---
if(APPLE)
    find_package(Eigen3 REQUIRED HINTS /opt/homebrew/share/eigen3 /opt/homebrew/include/eigen3)
else()
    find_package(Eigen3 REQUIRED)
endif()

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

# === Testing Setup ===
enable_testing()

include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/tests/*.cpp)
add_executable(test-runner ${TEST_SOURCES})
target_include_directories(test-runner PRIVATE
        ${Eigen3_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(test-runner PRIVATE Catch2::Catch2WithMain Eigen3::Eigen)

include(Catch)
catch_discover_tests(test-runner)