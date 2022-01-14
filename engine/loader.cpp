# include <engine/loader.hpp>
# include <engine/decoder/decoder_gta3.hpp>
# include <engine/instruction.hpp>
# include <engine/command.hpp>
# include <engine/memory.hpp>

namespace idascm
{
    namespace
    {
        auto is_goto(command const & cmd) noexcept -> bool
        {
            if (! (cmd.flags & (command_flag_jump | command_flag_stop)))
                return false;
            if (cmd.flags & command_flag_conditional)
                return false;
            if (cmd.argument_count != 1)
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

    auto loader::load(void) -> bool
    {
        if (! load_header())
            return false;
        if (! load_layout())
            return false;
        return true;
    }

    auto loader::load_header(void) -> bool
    {
        std::int32_t address = 0;
        instruction ins;
        if (auto size = m_decoder->decode_instruction(address, ins))
        {
            // global variables
            m_header.address_globals = address;
            if (is_goto(ins) && to_int(ins.operand_list[0], address))
            {
                if (auto size = m_decoder->decode_instruction(address, ins))
                {
                    // objects
                    m_header.address_objects = address;
                    if (is_goto(ins) && to_int(ins.operand_list[0], address))
                    {
                        if (auto size = m_decoder->decode_instruction(address, ins))
                        {
                            // missions
                            m_header.address_missions = address;
                            if (is_goto(ins) && to_int(ins.operand_list[0], address))
                            {
                                if (auto size = m_decoder->decode_instruction(address, ins))
                                {
                                    // main
                                    m_header.address_main = address;
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    auto loader::load_layout(void) -> bool
    {
        std::uint32_t ptr = m_header.address_missions + 8;
        ptr += m_memory_api->read(ptr, &m_header.main_size);
        ptr += m_memory_api->read(ptr, &m_header.mission_size);
        ptr += m_memory_api->read(ptr, &m_header.mission_count);
        ptr += m_memory_api->read(ptr, &m_header.mission_script_count);

        m_layout.segments.reserve(m_header.mission_count + 1);
        auto main = segment \
        {
            m_header.base,
            std::numeric_limits<std::uint32_t>::max(),
            segment_type::mixed,
            "main"
        };
        m_layout.segments.push_back(main);

        for (std::uint32_t i = 0; i < m_header.mission_count; ++ i)
        {
            std::uint32_t address = 0;
            if (sizeof(address) != m_memory_api->read(ptr, &address))
                return false;
            m_layout.segments.back().size = address - m_layout.segments.back().address;
            char name[32] = {};
            std::snprintf(name, sizeof(name) - 1, "mission:%02u", i);
            auto mission = segment \
            {
                address,
                std::numeric_limits<std::uint32_t>::max(),
                segment_type::code,
                name,
            };
            m_layout.segments.push_back(mission);
            ptr += sizeof(address);
        }

        return true;
    }
}
