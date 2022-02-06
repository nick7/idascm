set (PROJECT engine)
add_library (
    ${PROJECT}
    STATIC
        # headers
        argument.hpp
        engine.hpp
        command.hpp
        command_manager.hpp
        command_set.hpp
        decoder.hpp
        gta3/decoder_gta3.hpp
        gta3/loader_gta3.hpp
        gtalcs/decoder_gtalcs.hpp
        gtalcs/loader_gtalcs.hpp
        gtasa/decoder_gtasa.hpp
        gtasa/loader_gtasa.hpp
        gtavc/decoder_gtavc.hpp
        gtavcs/decoder_gtavcs.hpp
        instruction.hpp
        loader.hpp
        memory.hpp
        operand.hpp
        version.hpp
        # sources
        argument.cpp
        command.cpp
        command_manager.cpp
        command_set.cpp
        decoder.cpp
        gta3/decoder_gta3.cpp
        gta3/loader_gta3.cpp
        gtalcs/decoder_gtalcs.cpp
        gtalcs/loader_gtalcs.cpp
        gtasa/decoder_gtasa.cpp
        gtasa/loader_gtasa.cpp
        gtavc/decoder_gtavc.cpp
        gtavcs/decoder_gtavcs.cpp
        instruction.cpp
        loader.cpp
        memory.cpp
        operand.cpp
        version.cpp
)
target_link_libraries (
    ${PROJECT}
    PUBLIC
        core
)
