# include <engine/gtasa/loader_gtasa.hpp>
# include <engine/gtasa/decoder_gtasa.hpp>
# include <engine/command.hpp>
# include <engine/instruction.hpp>
# include <engine/memory.hpp>

namespace idascm
{
    // Segment layout:
    //  globals
    //  objects
    //  missions
    //  scripts
    //  unk1
    //  unk2
    //  main
    //  mission:00
    //  ...
    //  mission:n-1

    namespace
    {
        auto is_goto(command const & cmd) noexcept -> bool
        {
            if (! (cmd.flags & (command_flag_branch | command_flag_stop)))
                return false;
            if (cmd.flags & command_flag_conditional)
                return false;
            if (cmd.arguments.size() != 1)
                return false;
            return true;
        }

        auto is_goto(instruction const & ins) noexcept -> bool
        {
            if (! ins.command || ! is_goto(*ins.command))
                return false;
            if (ins.operand_count != 1)
                return false;
            return true;
        }
    }

    // virtual
    auto loader_gtasa::load_header(void) -> bool
    {
        std::int32_t address = 0;
        std::size_t  count   = 0;
        for (std::size_t index = 0; index < 7; ++ index)
        {
            instruction ins = {};
            auto const size = m_decoder.decode_instruction(address, ins);
            if (! size)
                return false;
            memory_reader reader(m_memory, address + size);
            switch (index)
            {
                case 0:
                    m_header.address_globals = address;
                    break;
                case 1:
                {
                    m_header.address_objects = address;
                    std::uint8_t id;
                    if (! reader.read(id) || id != 0)
                        return false;
                    break;
                }
                case 2:
                {
                    m_header.address_missions = address;
                    std::uint8_t id;
                    if (! reader.read(id) || id != 1)
                        return false;
                    std::uint32_t main_size;
                    if (! reader.read(main_size) || ! main_size)
                        return false;
                    std::uint32_t mission_size;
                    if (! reader.read(mission_size))
                        return false;
                    std::uint32_t mission_count;
                    if (! reader.read(mission_count))
                        return false;
                    std::uint32_t mission_script_count;
                    if (! reader.read(mission_script_count))
                        return false;
                    m_header.missions.main_size             = main_size;
                    m_header.missions.mission_size          = mission_size;
                    m_header.missions.mission_count         = mission_count;
                    m_header.missions.mission_script_count  = mission_script_count;
                    m_header.missions.mission_table_address = reader.pointer();
                    break;
                }
                case 3:
                    m_header.address_scripts = address;
                    break;
                case 4:
                    m_header.address_unk1 = address;
                    break;
                case 5:
                    m_header.address_unk2 = address;
                    break;
                case 6:
                    m_header.address_start = address;
                    break;
            }
            if (! is_goto(ins) || ! to_int(ins.operand_list[0], address))
                break;
            ++ count;
        }

        if (count != 6)
        {
            m_header.base = std::numeric_limits<std::uint32_t>::max();
            return false;
        }

        m_header.base = 0;
        return true;
    }

    auto loader_gtasa::load_header_layout(void) -> bool
    {
        auto const append_segment = [this](segment_type type, char const * name, std::uint32_t address, std::uint32_t size = std::numeric_limits<std::uint32_t>::max())
        {
            auto seg = segment \
            {
                address,
                size,
                type,
                name
            };
            if (m_layout.segments.size())
                m_layout.segments.back().size = address - m_layout.segments.back().address;
            m_layout.segments.push_back(seg);
        };

        append_segment(segment_type::globals,   "globals",   m_header.address_globals);
        append_segment(segment_type::readonly,  "objects",   m_header.address_objects);
        append_segment(segment_type::readonly,  "missions",  m_header.address_missions);
        append_segment(segment_type::readonly,  "scripts",   m_header.address_scripts);
        append_segment(segment_type::readonly,  "unk1",      m_header.address_unk1);
        append_segment(segment_type::readonly,  "unk2",      m_header.address_unk2);
        append_segment(segment_type::code,      "main",      m_header.address_start, m_header.missions.main_size - (m_header.address_start - m_header.base));
        return true;
    }
}
