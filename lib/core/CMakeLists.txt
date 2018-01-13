project(pc_stencil_core C)

set(SOURCES
        pc_stencil/core.c
        pc_stencil/core.h
)

add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
