cmake_minimum_required(VERSION 3.21)

# Checks if list of files to process is provided in the SOURCE_CODE variable
if (SOURCE_CODE)
    # Tries to find cpplint command for different systems; the search result is
    # in the CPPLINT_FOUND variable
    if (WIN32)
        execute_process(
                COMMAND cmd /c "(help cpplint > nul || exit 0) && where cpplint > nul 2> nul"
                RESULT_VARIABLE CPPLINT_FOUND
                ERROR_QUIET
                OUTPUT_QUIET)
    elseif (UNIX)
        execute_process(
                COMMAND bash -c "command -v cpplint &> /dev/null"
                RESULT_VARIABLE CPPLINT_FOUND
                ERROR_QUIET
                OUTPUT_QUIET)
    endif ()

    # Checks cpplint search result
    if (CPPLINT_FOUND EQUAL 0)
        # Uses 'Utilities' package for the TARGET_NAME variable
        find_package(Utilities REQUIRED)

        # Sets name for target that will invoke the cpplint command
        set(CPPLINT_TARGET_NAME ${TARGET_NAME}_cpplint)

        # Tries to find the cmake script 'CheckCodeStyle.cmake' the new target
        # will execute
        find_file(CHECK_CODE_STYLE_SCRIPT_PATH CheckCodeStyle.cmake
                PATHS ${CMAKE_MODULE_PATH})
        if (${CHECK_CODE_STYLE_SCRIPT_PATH} EQUAL CHECK_CODE_STYLE_SCRIPT_PATH-NOTFOUND)
            message(FATAL_ERROR "The 'CheckCodeStyle.cmake' script is not found")
        endif ()

        # Checks if given file list has header file: if it is true that means
        # the 'include' directory must exist and this directory must consist the
        # other called as a current target; otherwise there is no header file,
        # so 'include' directory is not required
        string(REGEX MATCH "(.hpp;)|(.h;)" HEADERS_SEARCH_RESULT "${SOURCE_CODE}")
        if (HEADERS_SEARCH_RESULT AND NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/${TARGET_NAME})
            message(FATAL_ERROR "Invalid project structure: in the same directory where "
                    "'CMakeLists.txt' is placed there should be an 'include' directory inside "
                    "of which a '${TARGET_NAME}' directory should reside")
        endif ()

        string(REPLACE "/CheckCodeStyle.cmake" "" CPPLINT_TARGET_CACHE_DIR ${CHECK_CODE_STYLE_SCRIPT_PATH})
        set(CPPLINT_TARGET_CACHE_DIR ${CPPLINT_TARGET_CACHE_DIR}/cache/cpplint/${TARGET_NAME})
        file(WRITE ${CPPLINT_TARGET_CACHE_DIR} "")

        foreach(CURRENT_FILE ${SOURCE_CODE})
            file(WRITE ${CURRENT_FILE} "")
            file(TIMESTAMP ${CURRENT_FILE} LAST_MODIFICATION_TIME)
            file(APPEND ${CPPLINT_TARGET_CACHE_DIR} "${CURRENT_FILE}\n${LAST_MODIFICATION_TIME}\n")
        endforeach()

        # Creates a new target and binds execution of cmake script to it
        add_custom_target(${CPPLINT_TARGET_NAME}
                COMMAND ${CMAKE_COMMAND} -D SOURCE_CODE="${SOURCE_CODE}"
                -D WORKING_DIRECTORY=${CMAKE_SOURCE_DIR}
                -D PROJECT_DIRECTORY=${CMAKE_RELATIVE_CURRENT_SOURCE_DIR}
                -D HEADERS_SEARCH_RESULT=${HEADERS_SEARCH_RESULT}
                -D CPPLINT_TARGET_CACHE_DIR=${CPPLINT_TARGET_CACHE_DIR}
                -P ${CHECK_CODE_STYLE_SCRIPT_PATH})

        # Makes a current target dependent on the code style check target
        add_dependencies(${TARGET_NAME} ${CPPLINT_TARGET_NAME})

        message(STATUS "The '${CPPLINT_TARGET_NAME}' target has been successfully created")
    else ()
        message(FATAL_ERROR "cpplint is not found")
    endif ()
else ()
    message(FATAL_ERROR "Source code is not provided. Set a list of files to be "
            "processed by cpplint into a SOURCE_CODE variable")
endif ()
