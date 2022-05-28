cmake_minimum_required(VERSION 3.21)

# Checks for minimal C++ standard
find_package(Utilities REQUIRED)
check_cpp_min_standard(17)

enable_language(ASM)

# Sets files required to build the project and to be processed by cpplint
set(SOURCE_CODE
        ${SRC_DIR}/ExecutionContextDetails.S
        ${SRC_DIR}/Routine.cpp ${INCLUDE_DIR}/Routine.hpp
        ${SRC_DIR}/Scheduler.cpp ${INCLUDE_DIR}/Scheduler.hpp
        ${SRC_DIR}/StackPool.cpp ${INCLUDE_DIR}/StackPool.hpp)

add_library(${TARGET_NAME} ${SOURCE_CODE})

# Activates the usage of cpplint
find_package(Cpplint REQUIRED)

# Set include directories for this project build (PRIVATE) and for top-level
# projects (INTERFACE)
target_include_directories(${TARGET_NAME} PUBLIC ./include)