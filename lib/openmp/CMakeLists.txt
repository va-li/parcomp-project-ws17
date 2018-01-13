project(pc_stencil_openmp C)

set(SOURCES pc_stencil/openmp.c pc_stencil/openmp.h)

add_library(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} pc_stencil_core)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})