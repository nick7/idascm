set (PROJECT engine)
add_library (
    ${PROJECT}
    STATIC
        engine.hpp
        command.hpp
        command_manager.hpp
        command_set.hpp
        decoder.hpp
        version.hpp
        #
        command.cpp
        command_manager.cpp
        command_set.cpp
        decoder.cpp
        instruction.cpp
        version.cpp
)
target_link_libraries (
    ${PROJECT}
    PUBLIC
        core
)
