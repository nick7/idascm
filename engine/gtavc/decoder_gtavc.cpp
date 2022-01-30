# include <engine/gtavc/decoder_gtavc.hpp>
# include <engine/instruction.hpp>
# include <engine/memory.hpp>
# include <core/logger.hpp>
# include <cassert>

namespace idascm
{
    namespace
    {
        operand_type const gs_operand_type_table[] =
        {
            operand_type::none,
            operand_type::int32,
            operand_type::global,
            operand_type::local,
            operand_type::int8,
            operand_type::int16,
            operand_type::float32,
        };
    }

    auto to_operand_type(operand_type_gtavc internal_type) noexcept -> operand_type
    {
        if (to_uint(internal_type) < std::size(gs_operand_type_table))
            return gs_operand_type_table[to_uint(internal_type)];
        return operand_type::unknown;
    }

    auto decoder_gtavc::decode_operand_value(std::uint32_t address, operand_type_gtavc type, operand_value & value) const -> std::uint32_t
    {
        assert(m_memory);
        auto reader = memory_reader(*m_memory, address);
        switch (type)
        {
            case operand_type_gtavc::none:
                return 0;
            case operand_type_gtavc::int8:
                if (! reader.read(value.int8))
                    return 0;
                break;
            case operand_type_gtavc::int16:
                if (! reader.read(value.int16))
                    return 0;
                break;
            case operand_type_gtavc::int32:
                if (! reader.read(value.int32))
                    return 0;
                break;
            case operand_type_gtavc::float32:
                if (! reader.read(value.float32))
                    return 0;
                break;
            case operand_type_gtavc::local:
            case operand_type_gtavc::global:
                if (! reader.read(value.uint16))
                    return 0;
                value.variable.address  = value.uint16;
                value.variable.type     = operand_type::unknown; // managed by opcode
                break;
        }
        return reader.pointer() - address;
    }

    // virtual
    auto decoder_gtavc::decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t
    {
        assert(m_memory);
        auto reader = memory_reader(*m_memory, address);
        operand_type_gtavc type;
        if (reader.read(type))
        {
            auto size = decode_operand_value(reader.pointer(), type, op.value);
            assert(size <= 0x08);
            if (to_operand_type(type) != operand_type::unknown)
            {
                reader.skip(size);
                op.size             = static_cast<std::uint8_t>(reader.pointer() - address);
                op.type_internal    = to_uint(type);
                op.type             = to_operand_type(type);
                return op.size;
            }
            IDASCM_LOG_W("unable to decode operand: 0x%02x", type);
        }
        return 0;
    }
}
