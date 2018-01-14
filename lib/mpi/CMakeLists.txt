project(pc_stencil_mpi C)

set(SOURCES main.c pc_stencil/mpi_stencil.c pc_stencil/mpi_stencil.h)

add_executable(${PROJECT_NAME} ${SOURCES})

# START MPI INCLUDE
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
target_link_libraries(${PROJECT_NAME} ${MPI_LIBRARIES})
if(MPI_COMPILE_FLAGS)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()
if(MPI_LINK_FLAGS)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()
# END MPI INCLUDE
# https://cmake.org/pipermail/cmake/2011-June/045037.html -- for link flags


target_link_libraries(${PROJECT_NAME} pc_stencil_core)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})