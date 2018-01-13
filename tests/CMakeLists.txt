project(pc_stencil_tests C)

add_executable(parser_test parser_test.c)

target_link_libraries(parser_test pc_stencil_core pc_stencil_sequential pc_stencil_openmp pc_stencil_cilk)
link_directories(parser_test ../resources)

add_executable(openmp_test openmp_test.c)

target_link_libraries(openmp_test pc_stencil_core pc_stencil_openmp)
