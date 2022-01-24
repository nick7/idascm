# include <engine/gta3/decoder_gta3.hpp>
# include <engine/instruction.hpp>
# include <core/logger.hpp>
# include <cassert>

namespace idascm
{
    namespace
    {
        operand_type const gs_operand_type_table[] =
        {
            /* 0x00 */  operand_type::none,
            /* 0x01 */  operand_type::int32,
            /* 0x02 */  operand_type::global,
            /* 0x03 */  operand_type::local,
            /* 0x04 */  operand_type::int8,
            /* 0x05 */  operand_type::int16,
            /* 0x06 */  operand_type::float16i,
        };
    }

    auto to_operand_type(operand_type_gta3 internal_type) noexcept -> operand_type
    {
        if (to_uint(internal_type) < std::size(gs_operand_type_table))
            return gs_operand_type_table[to_uint(internal_type)];
        return operand_type::unknown;
    }

    auto decoder_gta3::decode_operand_value(std::uint32_t address, operand_type_gta3 type, operand_value & value) const -> std::uint32_t
    {
        assert(m_memory);
        auto reader = memory_reader(*m_memory, address);
        switch (type)
        {
            case operand_type_gta3::none:
                return 0;
            case operand_type_gta3::int8:
                if (! reader.read(value.int8))
                    return 0;
                break;
            case operand_type_gta3::int16:
                if (! reader.read(value.int16))
                    return 0;
                break;
            case operand_type_gta3::int32:
                if (! reader.read(value.int32))
                    return 0;
                break;
            case operand_type_gta3::float16i:
                if (! reader.read(value.int16))
                    return 0;
                break;
            case operand_type_gta3::local:
            case operand_type_gta3::global:
                if (! reader.read(value.uint16))
                    return 0;
                value.variable.address  = value.uint16;
                value.variable.type     = operand_type::unknown; // managed by opcode
                break;
        }
        return reader.pointer() - address;
    }

    // virtual
    auto decoder_gta3::decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t
    {
        assert(m_memory);
        auto reader = memory_reader(*m_memory, address);
        operand_type_gta3 type;
        if (reader.read(type))
        {
            auto size = decode_operand_value(reader.pointer(), type, op.value);
            assert(size <= 0x08);
            if (to_operand_type(type) != operand_type::unknown)
            {
                assert(size < 0x100);
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
