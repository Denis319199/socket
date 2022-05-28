cmake_minimum_required(VERSION 3.21)

# Checks for minimal C++ standard
find_package(Utilities REQUIRED)
check_cpp_min_standard(17)

# Sets files required to build the project and to be processed by cpplint
set(SOURCE_CODE
        ${SRC_DIR}/Socket.cpp ${INCLUDE_DIR}/Socket.hpp
        ${INCLUDE_DIR}/Mutex.hpp
        ${INCLUDE_DIR}/Thread.hpp)

add_library(${TARGET_NAME} ${SOURCE_CODE})

# Activates the usage of cpplint
find_package(Cpplint REQUIRED)

# Set include directories for this project build (PRIVATE) and for top-level
# projects (INTERFACE)
target_include_directories(${TARGET_NAME} PUBLIC ./include)

# Adding required dependencies for different environments
if (WIN32)
    list(APPEND LINK_LIBRARIES wsock32 ws2_32)
    target_link_options(socket PRIVATE -static)
elseif (UNIX)
    list(APPEND LINK_LIBRARIES pthread)
endif ()
target_link_libraries(${TARGET_NAME} ${LINK_LIBRARIES})
