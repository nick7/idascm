# include <engine/decoder.hpp>
# include <engine/instruction.hpp>
# include <engine/command_set.hpp>
# include <core/logger.hpp>
# include <cassert>

namespace idascm
{
    namespace
    {
    }

    auto decoder::decode_instruction(std::uint32_t address, instruction & in) const -> std::uint32_t
    {
        assert(m_memory && m_isa);
        std::uint32_t ptr = address;
        ptr += m_memory->read(ptr, &in.opcode);
        in.address = address;
        in.command = m_isa->get_command(in.opcode);
        if (! in.command)
        {
            // no command - no decoding
            return 0;
        }
        in.operand_count = 0;
        for (std::uint8_t i = 0; i < in.command->argument_count; ++ i)
        {
            in.operand_list[i].offset = static_cast<std::uint8_t>(ptr - address);
            switch (in.command->argument_list[i])
            {
                case argument_type::string64:
                    in.operand_list[i].type         = operand_string;
                    in.operand_list[i].value_ptr    = ptr;
                    ptr += 8;
                    break;
                default:
                    ptr += decode_operand(ptr, in.operand_list[i]);
                    break;
            }
            ++ in.operand_count;
        }
        in.size = (ptr - address);
        return ptr - address;
    }

    auto decoder::decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t
    {
        assert(m_memory && m_isa);
        std::uint32_t ptr = address;
        ptr += m_memory->read(ptr, &op.type);
        switch (op.type)
        {
            case operand_int8:
                ptr += m_memory->read(ptr, &op.value_int8);
                break;
            case operand_int16:
                ptr += m_memory->read(ptr, &op.value_int16);
                break;
            case operand_int32:
                ptr += m_memory->read(ptr, &op.value_int32);
                break;
            case operand_float32:
                ptr += m_memory->read(ptr, &op.value_float32);
                break;
            case operand_global:
            {
                std::uint16_t var = -1;
                ptr += m_memory->read(ptr, &var);
                op.value_ptr = var;
                break;
            }
            default:
                IDASCM_LOG_W("unsupported operand type: %d", op.type);
                return 0;
        }
        return ptr - address;
    }
}
