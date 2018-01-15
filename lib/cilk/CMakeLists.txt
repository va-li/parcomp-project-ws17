project(pc_stencil_cilk C)

set(SOURCES pc_stencil/cilk_stencil.c pc_stencil/cilk_stencil.h)

add_library(${PROJECT_NAME} ${SOURCES})

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcilkplus")

target_link_libraries(${PROJECT_NAME} pc_stencil_core)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})