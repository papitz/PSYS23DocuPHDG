cmake_minimum_required(VERSION 3.25)
project(HeatMatrix)
find_package(OpenCV REQUIRED)
set(CMAKE_CXX_STANDARD 17)
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( HeatMatrix main.cpp )
target_link_libraries( HeatMatrix  ${OpenCV_LIBS})