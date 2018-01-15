project(pc_stencil_openmp C)

set(SOURCES pc_stencil/openmp.c pc_stencil/openmp.h)

find_package(OpenMP)
if (OPENMP_FOUND)
    message("OpenMP is supported")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -O3")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS} -lcilkrts")
else()
    message("OpenMP is not supported")
endif()

add_library(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} pc_stencil_core)

add_executable(${PROJECT_NAME}_exe ${SOURCES} main.c)
target_link_libraries(${PROJECT_NAME}_exe pc_stencil_core)


target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
add_custom_command(TARGET ${PROJECT_NAME}_exe COMMAND mkdir ../../exes/ -p && cp -f ${PROJECT_NAME}_exe ../../exes/${PROJECT_NAME} && echo "Copied the new executable")
