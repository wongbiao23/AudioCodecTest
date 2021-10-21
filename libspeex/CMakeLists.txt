
project(speex)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/speex
    ${CMAKE_CURRENT_SOURCE_DIR}/libspeex
)
file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
set(LIBSPEEX_DEFS -DEXPORT= -DFLOATING_POINT -DUSE_SMALLFT -DVAR_ARRAYS)

add_compile_options(${LIBSPEEX_DEFS})
add_library(${PROJECT_NAME} STATIC ${SRC})
target_compile_options(${PROJECT_NAME} INTERFACE ${LIBSPEEX_DEFS})
target_include_directories(${PROJECT_NAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
