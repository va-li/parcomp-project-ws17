cmake_minimum_required(VERSION 3.7)
project(parcomp-project-ws17 C)

set(CMAKE_C_STANDARD 11)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")

add_subdirectory(util)
add_subdirectory(lib)
add_subdirectory(tests)