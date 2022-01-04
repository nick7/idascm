set (PROJECT core)
add_library (
    ${PROJECT}
    STATIC
        # headers
        core.hpp
        json.hpp
        logger.hpp
        # sources
        core.cpp
        json.cpp
        logger.cpp
)
target_compile_definitions (
    ${PROJECT}
    PUBLIC
        IDASCM_GIT_REVISION="${GIT_REVISION}"
        IDASCM_BUILD_TYPE=$<CONFIG>
        IDASCM_BUILD_$<UPPER_CASE:$<CONFIG>>=1
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
