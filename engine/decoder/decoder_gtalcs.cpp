# include <engine/decoder/decoder_gtalcs.hpp>
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
        };
    }

    // static
    auto decoder_gtalcs::to_operand_type(value_type type) noexcept -> operand_type
    {
        if (to_uint(type) < std::size(gs_operand_type_table))
            return gs_operand_type_table[to_uint(type)];
        if (type >= value_type::timer_first && type <= value_type::timer_last)
            return operand_type::timer;
        if (type >= value_type::local_first && type <= value_type::local_last)
            return operand_type::local;
        if (type >= value_type::local_array_first && type <= value_type::local_array_last)
            return operand_type::local_array;
        if (type >= value_type::global_first && type <= value_type::global_last)
            return operand_type::global;
        if (type >= value_type::global_array_first && type <= value_type::global_array_last)
            return operand_type::global_array;
        return operand_type::unknown;
    }

    // virtual
    auto decoder_gtalcs::decode_operand_type(std::uint32_t address, operand_type & type) const -> std::uint32_t
    {
        value_type value_type;
        assert(m_memory);
        if (sizeof(value_type) == m_memory->read(address, &value_type))
        {
            type = to_operand_type(value_type);
            if (type != operand_type::unknown)
            {
                return sizeof(value_type);
            }
        }
        return 0;
    }

    // virtual
    auto decoder_gtalcs::decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t
    {
        auto ptr = address;
        value_type value_type;
        if (sizeof(value_type) != m_memory->read(address, &value_type))
            return 0;
        ptr += sizeof(value_type);
        op.type = to_operand_type(value_type);
        switch (op.type)
        {
            case operand_type::int0:
            case operand_type::float0:
                // std::memset(op.value_placeholder, 0, sizeof(op.value_placeholder));
                op.value_uint64 = 0;
                break;
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
            case operand_type::float8:
            {
                std::uint8_t value;
                ptr += m_memory->read(ptr, &value);
                op.value_uint32 = value << 24;
                break;
            }
            case operand_type::float16:
            {
                std::uint16_t value;
                ptr += m_memory->read(ptr, &value);
                op.value_uint32 = value << 16;
                break;
            }
            case operand_type::float24:
            {
                std::uint8_t value[3];
                ptr += m_memory->read(ptr, value, sizeof(value));
                op.value_uint32 = (value[0] << 8) | (value[1] << 16) | (value[2] << 24);
                break;
            }
            case operand_type::float16i:
            {
                ptr += m_memory->read(ptr, &op.value_int16);
                break;
            }
            case operand_type::global:
            {
                std::uint8_t block = to_uint(value_type) - to_uint(value_type::global_first);
                std::uint8_t slot;
                ptr += m_memory->read(ptr, &slot);
                op.value_address = 4 * ((block << 8) + slot);
                break;
            }
            case operand_type::global_array:
            {
                std::uint8_t block = to_uint(value_type) - to_uint(value_type::global_array_first);
                std::uint8_t slot;
                ptr += m_memory->read(ptr, &slot);
                ptr += m_memory->read(ptr, &op.array_index);
                ptr += m_memory->read(ptr, &op.array_size);
                op.array_address    = 4 * ((block << 8) + slot);
                break;
            }
            case operand_type::local:
            {
                op.value_address = to_uint(value_type) - to_uint(value_type::local_first);
                break;
            }
            case operand_type::local_array:
            {
                std::uint8_t base = to_uint(value_type) - to_uint(value_type::local_array_first);
                ptr += m_memory->read(ptr, &op.array_index);
                ptr += m_memory->read(ptr, &op.array_size);
                op.array_address = base;
                break;
            }
            case operand_type::timer:
            {
                op.value_address = to_uint(value_type) - to_uint(value_type::timer_first);
                break;
            }
            default:
            {
                IDASCM_LOG_W("unsupported operand type: %d (%d)", op.type, value_type);
                return 0;
            }
        }
        op.size = (ptr - address);
        return op.size;
    }
}
