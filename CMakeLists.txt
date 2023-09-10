cmake_minimum_required(VERSION 3.0)

project(a3)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_ALL "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_ALL} ${CMAKE_CXX_FLAGS_DEBUG}")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_ALL} ${CMAKE_CXX_FLAGS_RELEASE}")
endif()

# print out all the CMAKE_CXX_FLAGS
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

find_package(glm REQUIRED)
find_package(SDL2 REQUIRED)

add_library(viewer src/viewer.cpp)
target_link_libraries(viewer glm::glm SDL2::SDL2)

add_library(ray src/ray.cpp src/shape.cpp)
target_link_libraries(ray glm::glm)

# compile all examples
file(GLOB APP_SOURCES examples/*.cpp)

foreach( exampleFile ${APP_SOURCES} )
    get_filename_component( exampleName ${exampleFile} NAME )
    string( REPLACE ".cpp" "" example ${exampleName} )
    add_executable( ${example} ${exampleFile} )
    target_link_libraries( ${example} ray viewer )
  endforeach( exampleFile ${APP_SOURCES} )
