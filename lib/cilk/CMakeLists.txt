project(pc_stencil_cilk C)

set(SOURCES pc_stencil/cilk_stencil.c pc_stencil/cilk_stencil.h)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcilkplus -O3")

add_library(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} pc_stencil_core)

add_executable(${PROJECT_NAME}_exe ${SOURCES} main.c)
target_link_libraries(${PROJECT_NAME}_exe pc_stencil_core)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
add_custom_command(TARGET ${PROJECT_NAME}_exe COMMAND mkdir ../../exes/ -p && cp -f ${PROJECT_NAME}_exe ../../exes/${PROJECT_NAME} && echo "Copied the new executable")
