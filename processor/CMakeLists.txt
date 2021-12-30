set (PROJECT processor)
add_library (
    ${PROJECT}
    SHARED
        #
        analyzer.hpp
        emulator.hpp
        module.hpp
        output.hpp
        processor.hpp
        #
        analyzer.cpp
        emulator.cpp
        module.cpp
        output.cpp
        processor.cpp
)
target_include_directories (
    ${PROJECT}
    PUBLIC
        ${IDASDK_ROOT}/include/
        ${IDASDK_ROOT}/module/
)
set_target_properties (
    ${PROJECT}
    PROPERTIES
        OUTPUT_NAME "idascm"
)
target_link_libraries (
    ${PROJECT}
    PUBLIC
        engine
        core
    PRIVATE
        ${IDASDK_ROOT}/lib/x64_win_vc_32/ida.lib
)
target_compile_definitions (
    ${PROJECT}
    PUBLIC
        __NT__
        __IDP__
        __X64__
)
target_compile_options (
    ${PROJECT}
    PRIVATE
        /wd4244 # warning C4244: 'xxx': conversion from 'signed xxx' to 'unsigned xxx', possible loss of data 
        /wd4267 # warning C4267: 'xxx': conversion from 'signed xxx' to 'unsigned xxx', possible loss of data 
)