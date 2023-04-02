cmake_minimum_required(VERSION 3.0)

project(a3)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

add_library(viewer src/hw.cpp src/viewer.cpp)
target_link_libraries(viewer GLEW::GLEW glm::glm OpenGL::GL SDL2::SDL2)

add_executable(example src/example.cpp)
target_link_libraries(example viewer)

add_library(mesh src/mesh.cpp)
target_link_libraries(mesh viewer)

add_executable(e1 examples/e1.cpp)
target_link_libraries(e1 mesh)

add_executable(e2 examples/e2.cpp)
target_link_libraries(e2 mesh)

add_executable(e3 examples/e3.cpp)
target_link_libraries(e3 mesh)

add_executable(e4 examples/e4.cpp)
target_link_libraries(e4 mesh)

add_executable(e5 examples/e5.cpp)
target_link_libraries(e5 mesh)
