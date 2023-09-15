cmake_minimum_required(VERSION 3.16.3)
project(t_HeatMatrix CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0)
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(t_HeatMatrix ./t_HeatMatrix.cc ./t_heat_functions.cc ../src/HeatMatrix.cpp ../src/heat_functions.cpp)
target_link_libraries(t_HeatMatrix GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(t_HeatMatrix)
