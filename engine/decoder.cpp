# include <engine/decoder.hpp>
# include <engine/instruction.hpp>
# include <engine/command_set.hpp>
# include <engine/instruction.hpp>
# include <engine/memory.hpp>
# include <core/logger.hpp>
# include <cassert>

namespace idascm
{
    decoder::decoder(void)
        : m_isa(nullptr)
        , m_memory(nullptr)
    {}

    decoder::~decoder(void)
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
                    in.operand_list[op].type = operand_type::string64;
                    in.operand_list[op].size = m_memory->read(ptr, in.operand_list[op].value_string64, 8);
                    break;
                case argument_type::int8:
                    in.operand_list[op].type = operand_type::int8;
                    in.operand_list[op].size = m_memory->read(ptr, &in.operand_list[op].value_int8);
                    break;
                case argument_type::int32:
                    in.operand_list[op].type = operand_type::int32;
                    in.operand_list[op].size = m_memory->read(ptr, &in.operand_list[op].value_int32);
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
            auto max_operand_count = std::size(in.operand_list);
            if (in.command->flags & command_flag_function_call)
            {
                max_operand_count = 4 + in.operand_list[0].value_uint8 + in.operand_list[1].value_uint8;
            }
            while (op < max_operand_count)
            {
                auto type = operand_type::unknown;
                auto size = decode_operand_type(ptr, type);
                if (! size)
                {
                    IDASCM_LOG_W("decode_operand_type failed at 0x%08x", ptr);
                    return 0;
                }
                if (operand_type::none == type)
                {
                    ptr += size;
                    break;
                }
                in.operand_list[op].offset = static_cast<std::uint8_t>(ptr - address);
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
        if (auto size = decode_operand_type(ptr, op.type))
        {
            ptr += size;
        }
        else
        {
            return 0;
        }
        switch (op.type)
        {
            case operand_type::int8:
                ptr += m_memory->read(ptr, &op.value_int8);
                break;
            case operand_type::int16:
                ptr += m_memory->read(ptr, &op.value_int16);
                break;
            case operand_type::int32:
                ptr += m_memory->read(ptr, &op.value_int32);
                break;
            case operand_type::float32:
                ptr += m_memory->read(ptr, &op.value_float32);
                break;
            case operand_type::float16i:
                ptr += m_memory->read(ptr, &op.value_int16);
                break;
            case operand_type::global:
            case operand_type::local:
                ptr += m_memory->read(ptr, &op.value_int16);
                break;
            default:
                IDASCM_LOG_W("unsupported operand type: %d", op.type);
                return 0;
        }
        op.size = (ptr - address);
        return op.size;
    }
}
