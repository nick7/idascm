# include <engine/decoder.hpp>
# include <engine/gta3/decoder_gta3.hpp>
# include <engine/gtavc/decoder_gtavc.hpp>
# include <engine/gtasa/decoder_gtasa.hpp>
# include <engine/gtalcs/decoder_gtalcs.hpp>
# include <engine/gtavcs/decoder_gtavcs.hpp>
# include <engine/instruction.hpp>
# include <engine/command_set.hpp>
# include <engine/instruction.hpp>
# include <engine/memory.hpp>
# include <core/logger.hpp>
# include <cassert>

namespace idascm
{
    // static
    auto decoder::create(game id, command_set const & isa, memory_api & memory) -> decoder *
    {
        switch (id)
        {
            case game::gta3:
                return new decoder_gta3(isa, memory);
            case game::gtavc:
                return new decoder_gtavc(isa, memory);
            case game::gtasa:
                return new decoder_gtasa(isa, memory);
            case game::gtalcs:
                return new decoder_gtalcs(isa, memory);
            case game::gtavcs:
                return new decoder_gtavcs(isa, memory);
            default:
                break;
        }
        return nullptr;
    }

    decoder::decoder(command_set const & isa, memory_api & memory)
        : m_isa(isa)
        , m_memory(memory)
    {}

    decoder::~decoder(void)
    {
    }

    auto decoder::decode_instruction(std::uint32_t address, instruction & in) const -> std::uint32_t
    {
        auto reader = memory_reader(m_memory, address);
        std::uint16_t opcode = 0;
        if (! reader.read(opcode))
            return 0;
        in.opcode  = opcode & ~0x8000;
        in.flags   = (opcode & 0x8000) ? instruction_flag_not : 0;
        in.address = address;
        in.command = m_isa.get_command(in.opcode);
        if (! in.command)
        {
            IDASCM_LOG_D("unknown opcode: 0x%04x", in.opcode);
            // no command - no decoding
            return 0;
        }
        in.operand_count = 0;

        std::size_t op = 0;
        while (op < std::min(in.command->arguments.size(), std::size(in.operand_list)))
        {
            if (in.command->arguments[op].type == type::variadic)
                break;
            in.operand_list[op].offset  = static_cast<std::uint8_t>(reader.pointer() - address);
            if (in.command->arguments[op].operand_type != operand_type::unknown)
            {
                in.operand_list[op].type = in.command->arguments[op].operand_type;
                in.operand_list[op].size = decode_operand_value(reader.pointer(), in.operand_list[op].type, in.operand_list[op].value);
            }
            else
            {
                if (! decode_operand(reader.pointer(), in.operand_list[op]))
                    return 0;
            }
            reader.skip(in.operand_list[op].size);
            ++ op;
        }
        if (op < in.command->arguments.size() && in.command->arguments[op].type == type::variadic)
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
                        ++ op;
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
        assert(op < std::numeric_limits<std::uint8_t>::max());
        in.operand_count = static_cast<std::uint8_t>(op);
        in.size = static_cast<std::uint16_t>(reader.pointer() - address);
        // post-processing
        for (std::size_t i = 0; i < in.operand_count; ++ i)
        {
            switch (in.operand_list[i].type)
            {
                case operand_type::global:
                    if (operand_type::unknown == in.operand_list[i].value.variable.type && i < in.command->arguments.size())
                    {
                        auto const type = remove_constant(remove_reference(in.command->arguments[i].type));
                        switch (type)
                        {
                            case type::integer:
                                in.operand_list[i].value.variable.type = operand_type::int32;
                                break;
                            case type::real:
                                in.operand_list[i].value.variable.type = operand_type::float32;
                                break;
                            // case type::string:
                            //     in.operand_list[i].value.variable.type = operand_type::string;
                            //     break;
                        }
                    }
                    break;
                case operand_type::global_array:
                    if (operand_type::unknown == in.operand_list[i].value.array.type)
                    {

                    }
                    break;
            }
        }
        return reader.pointer() - address;
    }

    auto decoder::decode_operand_value(std::uint32_t address, operand_type type, operand_value & value) const -> std::uint32_t
    {
        auto reader = memory_reader(m_memory, address);
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
            case operand_type::uint8:
                if (! reader.read(value.uint8))
                    return 0;
                break;
            case operand_type::uint16:
                if (! reader.read(value.uint16))
                    return 0;
                break;
            case operand_type::uint32:
                if (! reader.read(value.uint32))
                    return 0;
                break;
            case operand_type::uint64:
                if (! reader.read(value.uint64))
                    return 0;
                break;
            case operand_type::float32:
                if (! reader.read(value.float32))
                    return 0;
                break;
            case operand_type::float64:
                if (! reader.read(value.float64))
                    return 0;
                break;
            case operand_type::string8:
                if (! reader.read(value.string8))
                    return 0;
                break;
            case operand_type::string16:
            case operand_type::string128:
            {
                value.string.address = reader.pointer();
                char buffer[256] = {};
                switch (type)
                {
                    case operand_type::string16:
                        if (! reader.read(buffer, 16))
                            return 0;
                        break;
                    case operand_type::string128:
                        if (! reader.read(buffer, 128))
                            return 0;
                        break;
                }
                value.string.length = static_cast<std::uint16_t>(std::strlen(buffer));
                break;
            }
            default:
                IDASCM_LOG_W("unsupported operand type: %d", type);
                return 0;
        }
        return reader.pointer() - address;
    }
}
