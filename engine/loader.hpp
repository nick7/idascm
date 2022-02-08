# pragma once
# include <engine/engine.hpp>
# include <engine/version.hpp>
# include <string>
# include <vector>

namespace idascm
{
    class decoder;
    class memory_device;

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
    auto segment_create(std::uint32_t address, std::uint32_t size, segment_type type, std::string name) noexcept -> segment;

    struct header_globals
    {
        std::uint32_t   base;
        std::uint32_t   size;               // in bytes
        std::uint32_t   save_table_base;    // GTA:LCS
        std::uint32_t   save_table_size;    // GTA:LCS
    };

    struct header_objects
    {
        std::uint32_t   base;
        std::uint32_t   table_base;
        std::uint32_t   table_size;
    };

    struct header_missions
    {
        std::uint32_t   base;
        std::uint32_t   main_size;
        std::uint32_t   mission_size;
        std::uint32_t   script_count;
        std::uint32_t   table_base;
        std::uint32_t   table_size;
    };

    struct header_scripts
    {
        std::uint32_t   base;
    };

    struct header
    {
        std::uint32_t       base;               // virtual memory base offset
        std::uint32_t       address_unk1;       // (GTA:SA)
        std::uint32_t       address_unk2;       // (GTA:SA)
        std::uint32_t       entry_point;        // actual entry point
        header_globals      globals;
        header_objects      objects;
        header_missions     missions;
        header_missions     scripts;
    };

    struct layout
    {
        std::vector<segment> segments;
    };
    
    struct mission_descriptor
    {
        std::int32_t    address;
    };

    struct object_descriptor
    {
        char name[32];
    };

    class loader
    {
        public:
            static auto create(game id, decoder & decoder, memory_device & memory) -> loader *;

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

            auto get_base(void) const noexcept -> std::uint32_t
            {
                return m_header.base;
            }

            auto get_mission_descriptor_address(std::uint32_t index) const noexcept -> std::uint32_t
            {
                if (index < m_header.missions.table_size)
                    return m_header.missions.table_base + index * sizeof(std::uint32_t);
                return invalid_address;
            }

            auto get_object_descriptor_address(std::uint32_t index) const noexcept -> std::uint32_t
            {
                if (index < m_header.objects.table_size)
                    return m_header.objects.table_base + index * 24;
                return invalid_address;
            }

        public:
            explicit loader(memory_device & memory)
                : m_memory(memory)
                , m_header({})
                , m_layout({})
            {}
    
        protected:
            memory_device & m_memory;
            header          m_header;
            layout          m_layout;
    };
}
