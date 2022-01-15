# include <engine/gtasa/decoder_gtasa.hpp>
# include <engine/instruction.hpp>
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
            operand_type::unknown,
            operand_type::unknown,
            operand_type::string64,
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
}
