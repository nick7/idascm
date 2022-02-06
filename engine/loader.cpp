# include <engine/loader.hpp>
# include <engine/command.hpp>
# include <engine/command_set.hpp>
# include <engine/decoder.hpp>
# include <engine/gta3/loader_gta3.hpp>
# include <engine/gtasa/decoder_gtasa.hpp>
# include <engine/gtasa/loader_gtasa.hpp>
# include <engine/gtalcs/loader_gtalcs.hpp>
# include <engine/instruction.hpp>
# include <engine/memory.hpp>

namespace idascm
{
    auto segment_create(std::uint32_t address, std::uint32_t size, segment_type type, std::string name) noexcept -> segment
    {
        segment seg = {};
        seg.address = address;
        seg.size    = size;
        seg.type    = type;
        seg.name    = std::move(name);
        return seg;
    }

    auto loader::create(game id, decoder & decoder, memory_api & memory) -> loader *
    {
        if (id != version_game(decoder.isa().get_version()))
            return nullptr;
        switch (id)
        {
            case game::gta3:
                return new loader_gta3(memory, decoder);
            case game::gtasa:
                return new loader_gtasa(memory, decoder);
            case game::gtalcs:
                return new loader_gtalcs(memory, decoder);
        }
        return nullptr;
    }

    auto loader::load(void) -> bool
    {
        m_header = {};
        m_header.base           = invalid_address;
        m_header.globals.base   = invalid_address;
        m_header.objects.base   = invalid_address;
        m_header.missions.base  = invalid_address;
        m_header.scripts.base   = invalid_address;
        if (! load_header())
            return false;
        if (! load_header_layout())
            return false;
        if (! load_mission_layout())
            return false;
        return true;
    }

    auto loader::load_mission_layout(void) -> bool
    {
        memory_reader reader(m_memory, m_header.missions.table_base);

        std::vector<segment> segments;
        segments.reserve(m_header.missions.table_size);

        char const * format = "mission:%u";
        if (m_header.missions.table_size >= 10)
        {
            format = "mission:%02u";
        }
        if (m_header.missions.table_size >= 100)
        {
            format = "mission:%03u";
        }
        for (std::uint32_t i = 0; i < m_header.missions.table_size; ++ i)
        {
            std::uint32_t address = 0;
            if (! reader.read(address))
                return false;
            if (i > 0)
            {
                segments.back().size = address - segments.back().address;
            }
            char name[32] = {};
            std::snprintf(name, sizeof(name) - 1, format, i);
            segments.push_back(segment_create(address, -1, segment_type::code, name));
        }

        m_layout.segments.insert(end(m_layout.segments), begin(segments), end(segments));
        return true;
    }
}
