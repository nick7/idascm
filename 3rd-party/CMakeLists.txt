set (PROJECT ida)
add_library (
    ${PROJECT}
    INTERFACE
)
target_link_libraries (
    ${PROJECT}
    INTERFACE
        ${IDASDK_ROOT}/lib/x64_win_vc_32/ida.lib
)
target_include_directories (
    ${PROJECT}
    INTERFACE
        ${IDASDK_ROOT}/include/
        ${IDASDK_ROOT}/module/
)
target_compile_definitions (
    ${PROJECT}
    INTERFACE
        __IDP__
        __X64__
        __AMD64__
)
if (WIN32)
    target_compile_definitions (
        ${PROJECT}
        INTERFACE
            __NT__
    )
endif ()
if (OPTION_EA64)
    target_compile_definitions (
        ${PROJECT}
        INTERFACE
            __EA64__
    )
endif ()
target_compile_options (
    ${PROJECT}
    INTERFACE
        /wd4244 # warning C4244: 'xxx': conversion from 'signed xxx' to 'unsigned xxx', possible loss of data 
        /wd4267 # warning C4267: 'xxx': conversion from 'signed xxx' to 'unsigned xxx', possible loss of data 
)
