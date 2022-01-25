# include <engine/gtavcs/decoder_gtavcs.hpp>
# include <engine/instruction.hpp>
# include <core/logger.hpp>
# include <cassert>

namespace idascm
{
    namespace
    {
        operand_type const gs_operand_type_table[] =
        {
            operand_type::none,
            operand_type::int0,
            operand_type::float0,
            operand_type::float8,
            operand_type::float16,
            operand_type::float24,
            operand_type::int32,
            operand_type::int8,
            operand_type::int16,
            operand_type::float32,
            operand_type::string,
        };
    }

    auto to_operand_type(operand_type_gtavcs type) noexcept -> operand_type
    {
        if (to_uint(type) < std::size(gs_operand_type_table))
            return gs_operand_type_table[to_uint(type)];
        if (type >= operand_type_gtavcs::timer_first && type <= operand_type_gtavcs::timer_last)
            return operand_type::timer;
        if (type >= operand_type_gtavcs::local_first && type <= operand_type_gtavcs::local_last)
            return operand_type::local;
        if (type >= operand_type_gtavcs::local_array_first && type <= operand_type_gtavcs::local_array_last)
            return operand_type::local_array;
        if (type >= operand_type_gtavcs::global_first && type <= operand_type_gtavcs::global_last)
            return operand_type::global;
        if (type >= operand_type_gtavcs::global_array_first && type <= operand_type_gtavcs::global_array_last)
            return operand_type::global_array;
        return operand_type::unknown;
    }

    auto decoder_gtavcs::decode_operand_value(std::uint32_t address, operand_type_gtavcs type, operand_value & value) const -> std::uint32_t
    {
        assert(m_memory);
        auto reader = memory_reader(*m_memory, address);
        switch (to_operand_type(type))
        {
            case operand_type::none:
                return 0;
            case operand_type::int0:
                value.int64 = 0;
                break;
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
            case operand_type::float0:
                value.int64 = 0;
                break;
            case operand_type::float8:
                if (! reader.read(value.uint8))
                    return 0;
                break;
            case operand_type::float16:
                if (! reader.read(value.uint16))
                    return 0;
                break;
            case operand_type::float24:
                std::uint8_t buffer[3];
                if (! reader.read(buffer, sizeof(buffer)))
                    return 0;
                value.uint32 = (buffer[0] << 0) | (buffer[1] << 8) | (buffer[2] << 16);
                break;
            case operand_type::float32:
                if (! reader.read(value.float32))
                    return 0;
                break;
            case operand_type::string:
            {
                value.string.address = reader.pointer();
                value.string.length  = 0;
                char sym = 0;
                while (true)
                {
                    if (! reader.read(sym))
                        return 0;
                    if (! sym)
                        break;
                    ++ value.string.length;
                }
                break;
            }
            case operand_type::global:
            {
                std::uint8_t block = to_uint(type) - to_uint(operand_type_gtavcs::global_first);
                std::uint8_t slot;
                if (! reader.read(slot))
                    return 0;
                value.variable.address = 4 * ((block << 8) + slot);
                break;
            }
            case operand_type::global_array:
            {
                std::uint8_t block = to_uint(type) - to_uint(operand_type_gtavcs::global_array_first);
                std::uint8_t slot, index, size;
                if (! reader.read(slot) || ! reader.read(index) || ! reader.read(size))
                    return 0;
                value.array.address     = 4 * ((block << 8) + slot);
                value.array.index       = index;
                value.array.size        = size;
                break;
            }
            case operand_type::local:
            {
                value.variable.address = to_uint(type) - to_uint(operand_type_gtavcs::local_first);
                break;
            }
            case operand_type::local_array:
            {
                std::uint8_t base = to_uint(type) - to_uint(operand_type_gtavcs::local_array_first);
                std::uint8_t index, size;
                if (! reader.read(index) || ! reader.read(size))
                    return 0;
                value.array.address = base;
                value.array.index   = index;
                value.array.size    = size;
                break;
            }
            case operand_type::timer:
            {
                value.variable.address  = to_uint(type) - to_uint(operand_type_gtavcs::timer_first);
                value.variable.type     = operand_type::timer;
                break;
            }
        }
        return reader.pointer() - address;
    }

    // virtual
    auto decoder_gtavcs::decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t
    {
        assert(m_memory);
        auto reader = memory_reader(*m_memory, address);
        operand_type_gtavcs type;
        if (reader.read(type))
        {
            auto size = decode_operand_value(reader.pointer(), type, op.value);
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
