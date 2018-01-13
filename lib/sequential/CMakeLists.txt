project(pc_stencil_sequential C)

set(SOURCES pc_stencil/sequential.c pc_stencil/sequential.h)

add_library(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} pc_stencil_core)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})