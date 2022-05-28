# Sets current target name
string(FIND ${CMAKE_CURRENT_SOURCE_DIR} "/" FIND_UTILITIES_DIR_START_POS REVERSE)
math(EXPR FIND_UTILITIES_DIR_START_POS "${FIND_UTILITIES_DIR_START_POS} + 1")
string(SUBSTRING ${CMAKE_CURRENT_SOURCE_DIR} ${FIND_UTILITIES_DIR_START_POS} -1 TARGET_NAME)
unset(FIND_UTILITIES_DIR_START_POS)

# Sets relative path to this CMakeLists.txt starting from top-level CMakeLists.txt
string(REPLACE ${CMAKE_SOURCE_DIR} "" CMAKE_RELATIVE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RELATIVE_CURRENT_SOURCE_DIR .${CMAKE_RELATIVE_CURRENT_SOURCE_DIR})

# Sets handy variables
set(SRC_DIR ./src)
set(INCLUDE_DIR ./include/${TARGET_NAME})

function(check_cpp_min_standard MIN_CPP_STANDARD)
    if ((CMAKE_CXX_STANDARD LESS ${MIN_CPP_STANDARD})
            OR (CMAKE_CXX_STANDARD_REQUIRED EQUAL False))
        message(FATAL_ERROR "To compile '${TARGET_NAME}', at least C++${MIN_CPP_STANDARD} "
                "standard is required; set the right values for CMAKE_CXX_STANDARD and "
                "CMAKE_CXX_STANDARD_REQUIRED variables")
    endif ()
endfunction()
