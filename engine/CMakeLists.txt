set (PROJECT engine)
add_library (
    ${PROJECT}
    STATIC
        # headers
        engine.hpp
        command.hpp
        command_manager.hpp
        command_set.hpp
        decoder.hpp
        decoder_gta3.hpp
        decoder_gtavc.hpp
        instruction.hpp
        version.hpp
        # sources
        command.cpp
        command_manager.cpp
        command_set.cpp
        decoder.cpp
        decoder_gta3.cpp
        decoder_gtavc.cpp
        instruction.cpp
        version.cpp
)
target_link_libraries (
    ${PROJECT}
    PUBLIC
        core
)
