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
        gta3/decoder_gta3.hpp
        gtalcs/decoder_gtalcs.hpp
        gtavc/decoder_gtavc.hpp
        instruction.hpp
        loader.hpp
        memory.hpp
        version.hpp
        # sources
        command.cpp
        command_manager.cpp
        command_set.cpp
        decoder.cpp
        gta3/decoder_gta3.cpp
        gtalcs/decoder_gtalcs.cpp
        gtavc/decoder_gtavc.cpp
        instruction.cpp
        loader.cpp
        memory.cpp
        version.cpp
)
target_link_libraries (
    ${PROJECT}
    PUBLIC
        core
)
