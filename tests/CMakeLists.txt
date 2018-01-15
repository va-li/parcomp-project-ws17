project(pc_stencil_tests C)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcilkplus")

find_package(OpenMP)
if (OPENMP_FOUND)
    message("OpenMP is supported")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS} -lcilkrts")
else()
    message("OpenMP is not supported")
endif()

add_executable(stencil-test stencil_test.c)
target_link_libraries(stencil-test pc_stencil_core pc_stencil_sequential pc_stencil_openmp pc_stencil_cilk)
link_directories(stencil-test ../resources)

add_executable(openmp_test openmp_test.c)
target_link_libraries(openmp_test pc_stencil_core pc_stencil_sequential pc_stencil_openmp pc_stencil_cilk)
link_directories(openmp_test ../resources)
