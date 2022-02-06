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
        memory_reader reader(m_memory, 0);
        std::int32_t base       = 0;
        std::int32_t address    = 0;
        std::size_t  count      = 0;
        for (std::size_t index = 0; index < 4; ++ index)
        {
            instruction ins = {};
            auto const size = m_decoder.decode_instruction(address, ins);
            if (! size)
                return false;
            if (! reader.set_pointer(base + address + size))
                return false;
            switch (index)
            {
                case 0:
                {
                    header_globals globals = {};
                    globals.base = address;
                    m_header.globals = globals;
                    break;
                }
                case 1:
                {
                    header_objects objects = {};
                    objects.base = address;
                    std::uint8_t id;
                    if (! reader.read(id))
                        return false;
                    std::uint32_t count;
                    if (! reader.read(count))
                        return false;
                    objects.table_size = count;
                    objects.table_base = reader.pointer();
                    m_header.objects = objects;
                    break;
                }
                case 2:
                {
                    header_missions missions = {};
                    missions.base = address;
                    std::uint8_t id;
                    if (! reader.read(id))
                        return false;
                    std::uint32_t main_size;
                    if (! reader.read(main_size))
                        return false;
                    std::uint32_t mission_size;
                    if (! reader.read(mission_size))
                        return false;
                    std::uint16_t mission_count;
                    if (! reader.read(mission_count))
                        return false;
                    std::uint16_t script_count;
                    if (! reader.read(script_count))
                        return false;
                    missions.main_size      = main_size;
                    missions.mission_size   = mission_size;
                    missions.script_count   = script_count;
                    missions.table_size     = mission_count;
                    missions.table_base     = reader.pointer();
                    break;
                }
                case 3:
                {
                    m_header.entry_point = address;
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
