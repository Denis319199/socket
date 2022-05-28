cmake_minimum_required(VERSION 3.21)

project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Sets files required to build the project and to be processed by cpplint
set(SOURCE_CODE Main.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_CODE})

# Activates the usage of cpplint
find_package(Cpplint)

add_subdirectory(libs/socket)
add_subdirectory(libs/fiber)

target_link_libraries(${PROJECT_NAME} socket fiber)