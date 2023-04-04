cmake_minimum_required(VERSION 3.0)

project(a3)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

find_package(glm REQUIRED)
find_package(SDL2 REQUIRED)

add_library(viewer src/viewer.cpp)
target_link_libraries(viewer glm::glm SDL2::SDL2)

add_library(ray src/ray.cpp src/shape.cpp)
target_link_libraries(ray glm::glm)

add_executable(e1 examples/e1.cpp)
target_link_libraries(e1 ray viewer)
