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

add_executable(e2 examples/e2.cpp)
target_link_libraries(e2 ray viewer)

add_executable(e3 examples/e3.cpp)
target_link_libraries(e3 ray viewer)

add_executable(e4 examples/e4.cpp)
target_link_libraries(e4 ray viewer)

add_executable(e5 examples/e5.cpp)
target_link_libraries(e5 ray viewer)

add_executable(e6 examples/e6.cpp)
target_link_libraries(e6 ray viewer)

add_executable(e7 examples/e7.cpp)
target_link_libraries(e7 ray viewer)

add_executable(e8 examples/e8.cpp)
target_link_libraries(e8 ray viewer)
