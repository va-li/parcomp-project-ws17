cmake_minimum_required(VERSION 3.7)
project(parcomp-project-ws17 C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

set(SOURCE_FILES src/main/c/core/core.c src/main/c/core/core.h src/main/c/sequential/sequential.c src/main/c/sequential/sequential.h)

add_executable(maintest src/test/c/test.c ${SOURCE_FILES})

add_executable(data-generator src/test/c/data_generator.c)