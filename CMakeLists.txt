cmake_minimum_required(VERSION 3.16.3)
project(HeatMatrix CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} include)

file(GLOB_RECURSE SOURCES "include/*.hpp" "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})
if(OpenMP_CXX_FOUND)
  target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX
                                               ${OpenCV_LIBS})
endif()
set(CMAKE_CXX_FLAGS "-O0")
set(CMAKE_C_FLAGS "-O0")
