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
        data,
    };

    struct segment
    {
        std::uint32_t   address;
        std::uint32_t   size;
        segment_type    type;
        std::string     name;
    };

    struct header
    {
        std::uint32_t   base;                   // virtual memory base offset
        std::uint32_t   address_globals;
        std::uint32_t   address_objects;
        std::uint32_t   address_missions;
        std::uint32_t   address_main;           // actual entry point
        std::uint32_t   main_size;
        std::uint32_t   mission_size;
        std::uint16_t   mission_count;
        std::uint16_t   mission_script_count;
    };

    struct layout
    {
        std::vector<segment> segments;
    };

    class loader
    {
        public:
            auto load(void) -> bool;
            auto load_header(void) -> bool;
            auto load_layout(void) -> bool;

            auto get_header(void) const -> header
            {
                return m_header;
            }

            auto get_layout(void) const -> layout
            {
                return m_layout;
            }
    
        public:
            explicit loader(memory_api * memory, decoder * decoder)
                : m_memory_api(memory)
                , m_decoder(decoder)
                , m_header({})
                , m_layout({})
            {}
    
        private:
            memory_api *    m_memory_api;
            decoder *       m_decoder;
            header          m_header;
            layout          m_layout;
    };
}
