cmake_minimum_required(VERSION 3.7)
project(parcomp-project-ws17 C)

set(CMAKE_C_STANDARD 11)

find_package(OpenMP)
if (OPENMP_FOUND)
    message("OpenMP is supported")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -O3")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else()
    message("OpenMP is not supported")
endif()

add_subdirectory(util)
add_subdirectory(lib)
add_subdirectory(tests)