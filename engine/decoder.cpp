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
        std::uint32_t ptr    = address;
        std::uint16_t opcode = 0;
        ptr += m_memory->read(ptr, &opcode);
        in.opcode  = opcode & ~0x8000;
        in.flags   = (opcode & 0x8000) ? instruction_flag_not : 0;
        in.address = address;
        in.command = m_isa->get_command(in.opcode);
        if (! in.command)
        {
            // no command - no decoding
            return 0;
        }
        in.operand_count = 0;

        std::uint8_t op = 0;
        while (op < in.command->argument_count)
        {
            if (in.command->argument_list[op] == argument_type::variadic)
                break;
            in.operand_list[op].offset  = static_cast<std::uint8_t>(ptr - address);
            switch (in.command->argument_list[op])
            {
                case argument_type::string64:
                    in.operand_list[op].type        = operand_string;
                    in.operand_list[op].value_ptr   = ptr;
                    in.operand_list[op].size = 8;
                    break;
                default:
                    if (! decode_operand(ptr, in.operand_list[op]))
                        return 0;
                    break;
            }
            ptr += in.operand_list[op].size;
            ++ op;
        }
        if (in.command->argument_list[op] == argument_type::variadic)
        {
            while (op < std::size(in.operand_list))
            {
                std::uint8_t type = 0;
                if (! m_memory->read(ptr, &type))
                {
                    IDASCM_LOG_W("memoey read failed at 0x%08x", ptr);
                    return 0;
                }
                if (operand_none == type)
                {
                    ptr += sizeof(type);
                    break;
                }
                in.operand_list[op].offset  = static_cast<std::uint8_t>(ptr - address);
                if (! decode_operand(ptr, in.operand_list[op]))
                    return 0;
                ptr += in.operand_list[op].size;
                ++ op;
            }
        }
        in.operand_count = op;
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
            case operand_local:
            {
                std::uint16_t var = -1;
                ptr += m_memory->read(ptr, &var);
                op.value_ptr = var;
                break;
            }
            default:
            {
                IDASCM_LOG_W("unsupported operand type: %d", op.type);
                return 0;
            }
        }
        op.size = (ptr - address);
        return op.size;
    }
}
