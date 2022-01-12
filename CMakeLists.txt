cmake_minimum_required (VERSION 3.14)

project ("idascm")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

option (OPTION_EA64 "64-bit Address Space" OFF)

execute_process (
    COMMAND
        git describe --long --tags --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if (NOT GIT_REVISION)
    execute_process (
        COMMAND
            git describe --long --all --dirty
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_REVISION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif ()
message (STATUS "GIT_REVISION: ${GIT_REVISION}")

add_subdirectory (3rd-party)

add_subdirectory (core)
add_subdirectory (engine)
add_subdirectory (ida)

add_subdirectory (tests)
