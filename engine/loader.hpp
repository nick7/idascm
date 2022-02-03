# pragma once
# include <engine/engine.hpp>
# include <engine/version.hpp>
# include <string>
# include <vector>

namespace idascm
{
    class decoder;
    class memory_api;

    enum class segment_type
    {
        unknown,
        mixed,
        code,
        globals,    // .bss
        readonly,   // .rodata
    };

    struct segment
    {
        std::uint32_t   address;
        std::uint32_t   size;
        segment_type    type;
        std::string     name;
    };

    struct header_missions
    {
        std::uint32_t   main_size;
        std::uint32_t   mission_size;
        std::uint32_t   mission_count;
        std::uint32_t   mission_script_count;
        std::uint32_t   mission_table_address;
    };

    struct header_objects
    {
        std::uint32_t   object_count;
        std::uint32_t   object_table_address;
    };

    struct header
    {
        std::uint32_t       base;               // virtual memory base offset
        std::uint32_t       address_globals;
        std::uint32_t       address_objects;
        std::uint32_t       address_missions;
        std::uint32_t       address_scripts;    // (GTA:SA)
        std::uint32_t       address_unk1;       // (GTA:SA)
        std::uint32_t       address_unk2;       // (GTA:SA)
        std::uint32_t       address_start;      // actual entry point
        header_objects      objects;
        header_missions     missions;
    };

    struct layout
    {
        std::vector<segment> segments;
    };

    class loader
    {
        public:
            static auto create(game id, decoder & decoder, memory_api & memory) -> loader *;

        public:
            virtual auto load(void) -> bool;
            virtual auto load_header(void) -> bool = 0;
            virtual auto load_header_layout(void) -> bool = 0;
            virtual auto load_mission_layout(void) -> bool;

            auto get_header(void) const -> header
            {
                return m_header;
            }

            auto get_layout(void) const -> layout
            {
                return m_layout;
            }
    
        public:
            explicit loader(memory_api & memory)
                : m_memory(memory)
                , m_header({})
                , m_layout({})
            {}
    
        protected:
            memory_api &    m_memory;
            header          m_header;
            layout          m_layout;
    };
}
