set (PROJECT ida_loader)
add_library (
    ${PROJECT}
    SHARED
        # headers
        loader.hpp
        # sources
        loader.cpp
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
            OUTPUT_NAME "${PROJECT_NAME}_ldr64"
    )
else ()
    set_target_properties (
        ${PROJECT}
        PROPERTIES
            OUTPUT_NAME "${PROJECT_NAME}_ldr"
    )
endif ()
