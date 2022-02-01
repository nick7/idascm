# include <engine/loader.hpp>
# include <engine/gta3/decoder_gta3.hpp>
# include <engine/instruction.hpp>
# include <engine/command.hpp>
# include <engine/memory.hpp>

namespace idascm
{
    auto loader::load(void) -> bool
    {
        m_header = header \
        {
            std::numeric_limits<std::uint32_t>::max(),
            std::numeric_limits<std::uint32_t>::max(),
            std::numeric_limits<std::uint32_t>::max(),
            std::numeric_limits<std::uint32_t>::max(),
            std::numeric_limits<std::uint32_t>::max(),
            std::numeric_limits<std::uint32_t>::max(),
            std::numeric_limits<std::uint32_t>::max(),
            std::numeric_limits<std::uint32_t>::max(),
        };
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
        memory_reader reader(m_memory, m_header.missions.mission_table_address);

        m_layout.segments.reserve(m_layout.segments.size() + m_header.missions.mission_count);
        for (std::uint32_t i = 0; i < m_header.missions.mission_count; ++ i)
        {
            std::uint32_t address = 0;
            if (! reader.read(address))
                return false;
            if (i > 0)
            {
                m_layout.segments.back().size = address - m_layout.segments.back().address;
            }
            char name[32] = {};
            std::snprintf(name, sizeof(name) - 1, "mission:%03u", i);
            auto mission = segment \
            {
                address,
                std::numeric_limits<std::uint32_t>::max(),
                segment_type::code,
                name,
            };
            m_layout.segments.push_back(mission);
        }
        return true;
    }
}
