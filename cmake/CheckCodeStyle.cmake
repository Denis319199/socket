string(REPLACE " " ";" SOURCE_CODE ${SOURCE_CODE})
list(TRANSFORM SOURCE_CODE REPLACE "^\\." "")
list(TRANSFORM SOURCE_CODE REPLACE "^/" "")
list(TRANSFORM SOURCE_CODE PREPEND ${PROJECT_DIRECTORY}/)

if (HEADERS_SEARCH_RESULT)
    execute_process(
            COMMAND cpplint --repository=${PROJECT_DIRECTORY}/include ${SOURCE_CODE}
            WORKING_DIRECTORY ${WORKING_DIRECTORY}
            RESULT_VARIABLE CPPLINT_EXIT_CODE
    )
else ()
    execute_process(
            COMMAND cpplint ${SOURCE_CODE}
            WORKING_DIRECTORY ${WORKING_DIRECTORY}
            RESULT_VARIABLE CPPLINT_EXIT_CODE
    )
endif ()

if (NOT ${CPPLINT_EXIT_CODE} EQUAL 0)
    message(FATAL_ERROR "Code style is not complied")
endif ()
