
project(gsm)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC)

add_library(${PROJECT_NAME} STATIC ${SRC})
target_include_directories(${PROJECT_NAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
