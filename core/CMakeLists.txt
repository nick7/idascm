set (PROJECT core)
add_library (
    ${PROJECT}
    STATIC
        json.hpp
        logger.hpp
        #
        json.cpp
        logger.cpp
)
target_include_directories (
    ${PROJECT}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../
)
if (MSVC)
    target_compile_definitions (
        ${PROJECT}
        PUBLIC
            _CRT_SECURE_NO_WARNINGS
    )
endif ()
