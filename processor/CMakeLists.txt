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
target_link_libraries (
    ${PROJECT}
    PUBLIC
        engine
        core
        ida
)
if (OPTION_EA64)
    set_target_properties (
        ${PROJECT}
        PROPERTIES
            OUTPUT_NAME "idascm64"
    )
else ()
    set_target_properties (
        ${PROJECT}
        PROPERTIES
            OUTPUT_NAME "idascm"
    )
endif ()
