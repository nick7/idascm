# include <engine/gtasa/decoder_gtasa.hpp>
# include <engine/instruction.hpp>
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
            /* 0x09 */  operand_type::string64,
            /* 0x0a */  operand_type::unknown,
            /* 0x0b */  operand_type::unknown,
            /* 0x0c */  operand_type::unknown,
            /* 0x0d */  operand_type::unknown,
            /* 0x0e */  operand_type::string,
        };
    }

    // virtual
    auto decoder_gtasa::decode_operand_type(std::uint32_t address, operand_type & type) const -> std::uint32_t
    {
        std::uint8_t value_type = -1;
        assert(m_memory);
        m_memory->read(address, &value_type);
        if (value_type < std::size(gs_operand_type_table))
        {
            type = gs_operand_type_table[value_type];
            return sizeof(value_type);
        }
        return 0;
    }

    // virtual
    auto decoder_gtasa::decode_operand_value(std::uint32_t address, operand_type type, operand_value & value) const -> std::uint32_t
    {
        std::uint32_t ptr = address;
        switch (type)
        {
            case operand_type::string:
            {
                std::uint8_t length = 0;
                ptr += m_memory->read(ptr, &length);
                value.string.length  = length;
                value.string.address = ptr;
                ptr += length;
                break;
            }
            default:
            {
                ptr += decoder::decode_operand_value(ptr, type, value);
                break;
            }
        }
        return ptr - address;
    }

    // virtual
    auto decoder_gtasa::decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t
    {
        assert(m_memory && m_isa);
        return decoder::decode_operand(address, op);
    }
}
