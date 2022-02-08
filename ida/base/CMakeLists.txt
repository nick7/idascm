set (PROJECT ida_base)
add_library (
    ${PROJECT}
    STATIC
        # headers
        base.hpp
        memory_ida.hpp
        memory_loader.hpp
        # sources
        base.cpp
        memory_ida.cpp
        memory_loader.cpp
)
target_link_libraries (
    ${PROJECT}
    PUBLIC
        engine
        core
        ida
)
