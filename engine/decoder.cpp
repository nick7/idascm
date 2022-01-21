# include <engine/decoder.hpp>
# include <engine/instruction.hpp>
# include <engine/command_set.hpp>
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
        auto reader = memory_reader(*m_memory, address);
        std::uint16_t opcode = 0;
        if (! reader.read(opcode))
            return 0;
        in.opcode  = opcode & ~0x8000;
        in.flags   = (opcode & 0x8000) ? instruction_flag_not : 0;
        in.address = address;
        in.command = m_isa->get_command(in.opcode);
        if (! in.command)
        {
            IDASCM_LOG_D("unknown opcode: 0x%04x", in.opcode);
            // no command - no decoding
            return 0;
        }
        in.operand_count = 0;

        std::uint8_t op = 0;
        while (op < in.command->argument_count)
        {
            if (in.command->argument_list[op] == argument_type::variadic)
                break;
            in.operand_list[op].offset  = static_cast<std::uint8_t>(reader.pointer() - address);
            switch (in.command->argument_list[op])
            {
                case argument_type::string64:
                    in.operand_list[op].type = operand_type::string8;
                    in.operand_list[op].size = decode_operand_value(reader.pointer(), in.operand_list[op].type, in.operand_list[op].value);
                    break;
                case argument_type::int8:
                    in.operand_list[op].type = operand_type::int8;
                    in.operand_list[op].size = decode_operand_value(reader.pointer(), in.operand_list[op].type, in.operand_list[op].value);
                    break;
                case argument_type::int16:
                    in.operand_list[op].type = operand_type::int16;
                    in.operand_list[op].size = decode_operand_value(reader.pointer(), in.operand_list[op].type, in.operand_list[op].value);
                    break;
                case argument_type::int32:
                    in.operand_list[op].type = operand_type::int32;
                    in.operand_list[op].size = decode_operand_value(reader.pointer(), in.operand_list[op].type, in.operand_list[op].value);
                    break;
                default:
                    if (! decode_operand(reader.pointer(), in.operand_list[op]))
                        return 0;
                    break;
            }
            reader.skip(in.operand_list[op].size);
            ++ op;
        }
        if (in.command->argument_list[op] == argument_type::variadic)
        {
            auto max_operand_count = std::size(in.operand_list);
            if (in.command->flags & command_flag_function_call)
            {
                max_operand_count = 4 + in.operand_list[0].value.uint8 + in.operand_list[1].value.uint8;
            }
            while (op < max_operand_count)
            {
                if (auto size = decode_operand(reader.pointer(), in.operand_list[op]))
                {
                    reader.skip(size);
                    if (operand_type::none == in.operand_list[op].type)
                    {
                        break;
                    }
                }
                else
                {
                    IDASCM_LOG_W("decode_operand_type failed at 0x%08x", reader.pointer());
                }
                ++ op;
            }
        }
        in.operand_count = op;
        in.size = static_cast<std::uint16_t>(reader.pointer() - address);
        return reader.pointer() - address;
    }

    auto decoder::decode_operand_value(std::uint32_t address, operand_type type, operand_value & value) const -> std::uint32_t
    {
        assert(m_memory);
        auto reader = memory_reader(*m_memory, address);
        switch (type)
        {
            case operand_type::int8:
                if (! reader.read(value.int8))
                    return 0;
                break;
            case operand_type::int16:
                if (! reader.read(value.int16))
                    return 0;
                break;
            case operand_type::int32:
                if (! reader.read(value.int32))
                    return 0;
                break;
            case operand_type::int64:
                if (! reader.read(value.int64))
                    return 0;
                break;
            case operand_type::float32:
                if (! reader.read(value.float32))
                    return 0;
                break;
            case operand_type::string8:
                if (! reader.read(value.string8))
                    return 0;
                break;
            default:
                IDASCM_LOG_W("unsupported operand type: %d", type);
                return 0;
        }
        return reader.pointer() - address;
    }
}
