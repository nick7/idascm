set (PROJECT ida_utils)
add_library (
    ${PROJECT}
    STATIC
        # headers
        memory_api_loader.hpp
        utils.hpp
        # sources
        utils.cpp
)
target_link_libraries (
    ${PROJECT}
    PUBLIC
        engine
        core
        ida
)
