set (PROJECT engine)
add_library (
    ${PROJECT}
    STATIC
        # headers
        engine.hpp
        command.hpp
        command_manager.hpp
        command_set.hpp
        decoder/decoder.hpp
        decoder/decoder_gta3.hpp
        decoder/decoder_gtavc.hpp
        instruction.hpp
        version.hpp
        # sources
        command.cpp
        command_manager.cpp
        command_set.cpp
        decoder/decoder.cpp
        decoder/decoder_gta3.cpp
        decoder/decoder_gtavc.cpp
        instruction.cpp
        version.cpp
)
target_link_libraries (
    ${PROJECT}
    PUBLIC
        core
)
