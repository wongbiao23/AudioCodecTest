
cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_ROOT}/out/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_ROOT}/out/libs)

project(code)

add_executable(${PROJECT_NAME} code.c)

target_link_libraries(${PROJECT_NAME} speex)
add_dependencies(${PROJECT_NAME} speex)
target_link_libraries(${PROJECT_NAME} gsm)
add_dependencies(${PROJECT_NAME} gsm)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libgsm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libspeex)
