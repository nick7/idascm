# pragma once
# include <engine/engine.hpp>

namespace idascm
{
    struct command;

    enum operand_type : std::uint8_t
    {
        operand_none        = 0x00,
        operand_int32       = 0x01,
        operand_global      = 0x02,
        operand_local       = 0x03,
        operand_int8        = 0x04,
        operand_int16       = 0x05,
        operand_float32     = 0x06,
        operand_string      = 0x10, // special
    };

    struct operand
    {
        std::uint8_t    type;
        std::uint8_t    offset;
        union
        {
            std::int8_t     value_int8;
            std::int16_t    value_int16;
            std::int32_t    value_int32;
            float           value_float32;
            char            value_string64[8];
            vmsize_t        value_ptr;
        };
    };

    enum instruction_flag : std::uint8_t
    {
        instruction_flag_not    = 1 << 0, // condition inversion
    };

    // instruction is a command instance with actual runtime values
    struct instruction
    {
        command const *     command;
        vmsize_t            address;
        std::uint16_t       opcode;
        std::uint8_t        flags;
        std::uint8_t        size;
        std::uint8_t        operand_count;
        operand             operand_list[16];
    };

    auto instruction_name(instruction const & ins) noexcept -> char const *;
}
