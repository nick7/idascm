# include <engine/gtasa/decoder_gtasa.hpp>
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
            /* 0x06 */  operand_type::float32,
            /* 0x07 */  operand_type::global_array,
            /* 0x08 */  operand_type::local_array,
            /* 0x09 */  operand_type::string8,
            /* 0x0a */  operand_type::global,       // string8
            /* 0x0b */  operand_type::local,        // string8
            /* 0x0c */  operand_type::global_array, // string8
            /* 0x0d */  operand_type::local_array,  // string8
            /* 0x0e */  operand_type::string,
            /* 0x0f */  operand_type::string16,     // string16
            /* 0x10 */  operand_type::global,       // string16
            /* 0x11 */  operand_type::local,        // string16
            /* 0x12 */  operand_type::global_array, // string16
            /* 0x13 */  operand_type::local_array,  // string16
        };
    }

    auto to_operand_type(operand_type_gtasa internal_type) noexcept -> operand_type
    {
        if (to_uint(internal_type) < std::size(gs_operand_type_table))
            return gs_operand_type_table[to_uint(internal_type)];
        return operand_type::unknown;
    }

    auto decoder_gtasa::decode_operand_value(std::uint32_t address, operand_type_gtasa type, operand_value & value) const -> std::uint32_t
    {
        assert(m_memory);
        auto reader = memory_reader(*m_memory, address);
        switch (type)
        {
            case operand_type_gtasa::none:
                return 0;
            case operand_type_gtasa::int8:
                if (! reader.read(value.int8))
                    return 0;
                break;
            case operand_type_gtasa::int16:
                if (! reader.read(value.int16))
                    return 0;
                break;
            case operand_type_gtasa::int32:
                if (! reader.read(value.int32))
                    return 0;
                break;
            case operand_type_gtasa::float32:
                if (! reader.read(value.float32))
                    return 0;
                break;
            case operand_type_gtasa::global:
            case operand_type_gtasa::local:
                if (! reader.read(value.uint16))
                    return 0;
                value.variable.address  = value.uint16;
                value.variable.type     = operand_type::unknown; // managed by opcode
                break;
            case operand_type_gtasa::string8:
                if (! reader.read(value.string8))
                    return 0;
                break;
            case operand_type_gtasa::string16:
                value.string.length  = 16;
                value.string.address = reader.pointer();
                reader.skip(16);
                break;
            case operand_type_gtasa::string:
            {
                std::uint8_t length = 0;
                if (! reader.read(length))
                    return 0;
                value.string.length  = length;
                value.string.address = reader.pointer();
                if (! reader.skip(length))
                    return 0;
                break;
            }
            case operand_type_gtasa::global_array:
            case operand_type_gtasa::local_array:
            case operand_type_gtasa::global_array_string8:
            case operand_type_gtasa::local_array_string8:
            case operand_type_gtasa::global_array_string16:
            case operand_type_gtasa::local_array_string16:
            {
                std::uint16_t off, index;
                if (! reader.read(off))
                    return 0;
                if (! reader.read(index))
                    return 0;
                std::uint8_t size, flags;
                if (! reader.read(size) || ! reader.read(flags))
                    return 0;
                // 0x00 - int
                // 0x01 - float
                value.array.address = off;
                value.array.index   = index;
                value.array.size    = size;
                value.array.flags   = flags;
                break;
            }
            case operand_type_gtasa::local_string8:
            case operand_type_gtasa::global_string8:
            {
                std::uint16_t address;
                if (! reader.read(address))
                    return 0;
                value.variable.address  = address;
                value.variable.type     = operand_type::string8;
                break;
            }
            case operand_type_gtasa::local_string16:
            case operand_type_gtasa::global_string16:
            {
                std::uint16_t address;
                if (! reader.read(address))
                    return 0;
                value.variable.address  = address;
                value.variable.type     = operand_type::string16;
                break;
            }
        }
        return reader.pointer() - address;
    }

    // virtual
    auto decoder_gtasa::decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t
    {
        assert(m_memory);
        auto reader = memory_reader(*m_memory, address);
        operand_type_gtasa type;
        if (reader.read(type))
        {
            if (auto size = decode_operand_value(reader.pointer(), type, op.value))
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
