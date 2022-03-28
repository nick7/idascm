set (PROJECT ida_processor)
add_library (
    ${PROJECT}
    SHARED
        # headers
        analyzer.hpp
        emulator.hpp
        module.hpp
        output.hpp
        processor.hpp
        # sources
        analyzer.cpp
        emulator.cpp
        module.cpp
        output.cpp
        processor.cpp
)
target_link_libraries (
    ${PROJECT}
    PUBLIC
        ida_base
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
install (
    TARGETS
        ${PROJECT}
    DESTINATION
        "procs"
)
if (MSVC)
    install (
        FILES
            $<TARGET_PDB_FILE:${PROJECT}>
        DESTINATION
            "procs"
        OPTIONAL
    )
endif ()
