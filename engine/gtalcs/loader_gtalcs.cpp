# include <engine/gtalcs/loader_gtalcs.hpp>
# include <engine/gtalcs/decoder_gtalcs.hpp>
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
    auto loader_gtalcs::load_header(void) -> bool
    {
        memory_reader reader(m_memory, 0);

        std::uint32_t main_size = 0;
        // if (! reader.read(main_size) || ! main_size)
        //     return false;
        // std::uint32_t mission_size = 0;
        // if (! reader.read(mission_size) || ! mission_size)
        //     return false;
        m_header.base = reader.pointer();
        std::int32_t address = 0;
        std::size_t  count   = 0;
        for (std::size_t index = 0; index < 4; ++ index)
        {
            instruction ins = {};
            auto const size = m_decoder.decode_instruction(address, ins);
            if (! size)
                return false;
            if (! reader.set_pointer(address + size))
                return false;
            switch (index)
            {
                case 0:
                {
                    header_globals globals = {};
                    globals.base = address;
                    std::int32_t next = 0;
                    if (to_int(ins.operand_list[0], next))
                    {
                        globals.size = next - globals.base;
                    }
                    m_header.globals = globals;
                    break;
                }
                case 1:
                {
                    // m_header.globals. = address;
                    std::uint8_t id;
                    if (! reader.read(id))
                        return false;
                    std::uint32_t save_table_size;
                    if (! reader.read(save_table_size))
                        return false;
                    if (save_table_size * sizeof(std::int32_t) > m_header.globals.size)
                        return false;
                    m_header.globals.save_table_size = save_table_size;
                    m_header.globals.save_table_base = reader.pointer();
                    if (! reader.skip(save_table_size * sizeof(std::uint16_t)))
                        return false;
                    header_objects objects = {};
                    objects.base = reader.pointer();
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
                    std::uint16_t true_global_count;
                    std::uint16_t most_global_count;
                    if (! reader.read(true_global_count) || ! reader.read(most_global_count))
                        return false;
                    std::uint32_t largest_mission_size;
                    std::uint16_t mission_count;
                    std::uint16_t exclusive_mission_count;
                    if (! reader.read(largest_mission_size))
                        return false;
                    if (! reader.read(mission_count) || ! reader.read(exclusive_mission_count))
                        return false;
                    missions.mission_size   = largest_mission_size;
                    missions.table_size     = mission_count;
                    missions.table_base     = reader.pointer();
                    m_header.missions = missions;
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

    auto loader_gtalcs::load_header_layout(void) -> bool
    {
        m_layout.segments.push_back
        (
            segment_create(0, m_header.entry_point, segment_type::mixed, "data")
        );
        m_layout.segments.push_back
        (
            segment_create
            (
                m_header.entry_point,
                m_header.missions.main_size - m_header.entry_point,
                segment_type::code,
                "main"
            )
        );
        return true;
    }
}
