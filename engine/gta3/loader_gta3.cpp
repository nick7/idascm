# include <engine/gta3/loader_gta3.hpp>
# include <engine/gta3/decoder_gta3.hpp>
# include <engine/command.hpp>
# include <engine/instruction.hpp>
# include <engine/memory.hpp>

namespace idascm
{
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
    auto loader_gta3::load_header(void) -> bool
    {
        std::int32_t address = 0;
        std::size_t  count   = 0;
        for (std::size_t index = 0; index < 4; ++ index)
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
                    if (! reader.read(id))
                        return false;
                    if (! reader.read(m_header.objects.object_count))
                        return false;
                    m_header.objects.object_table_address = reader.pointer();
                    break;
                }
                case 2:
                {
                    m_header.address_missions = address;
                    std::uint8_t id;
                    if (! reader.read(id))
                        return false;
                    if (! reader.read(m_header.missions.main_size))
                        return false;
                    if (! reader.read(m_header.missions.mission_size))
                        return false;
                    if (! reader.read(m_header.missions.mission_count))
                        return false;
                    if (! reader.read(m_header.missions.mission_script_count))
                        return false;
                    m_header.missions.mission_table_address = reader.pointer();
                    break;
                }
                case 3:
                {
                    m_header.address_start = address;
                    break;
                }
            }
            ++ count;
            if (! is_goto(ins) || ! to_int(ins.operand_list[0], address))
                break;
        }

        if (count != 4)
        {
            return false;
        }

        return true;
    }

    auto loader_gta3::load_header_layout(void) -> bool
    {
        return true;
    }
}
