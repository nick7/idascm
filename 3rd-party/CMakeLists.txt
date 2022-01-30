set (PROJECT jsmn)
file (
    GENERATE
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/jsmn/jsmn.c
    CONTENT
        "# undef JSMN_HEADER\n# include <jsmn.h>\n"
)
add_library (
    ${PROJECT}
        # headers
        jsmn/jsmn.h
        # generated
        ${CMAKE_CURRENT_BINARY_DIR}/jsmn/jsmn.c
)
target_compile_definitions (
    ${PROJECT}
    PUBLIC
        JSMN_PARENT_LINKS
        JSMN_HEADER
    PRIVATE
)
target_include_directories (
    ${PROJECT}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/jsmn/
)
