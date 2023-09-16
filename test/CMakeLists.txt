cmake_minimum_required(VERSION 3.16.3)
project(t_HeatMatrix CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} include)

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

add_executable(${PROJECT_NAME} ./t_HeatMatrix.cc ./t_heat_functions.cc
                               ../src/HeatMatrix.cpp ../src/heat_functions.cpp)
target_link_libraries(${PROJECT_NAME} GTest::gtest_main)
if(OpenMP_CXX_FOUND)
  target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX
                                               ${OpenCV_LIBS})
endif()

include(GoogleTest)
gtest_discover_tests(t_HeatMatrix)
