# pragma once
# include <engine/engine.hpp>

namespace idascm
{
    struct command;

    enum class operand_type // : std::uint8_t
    {
        unknown = -1,
        none,
        int8,           // immediate signed 8-bit integer
        int16,          // immediate signed 16-bit integer
        int32,          // immediate signed 32-bit integer
        int64,          // immediate signed 64-bit integer (reserved)
        global,         // global variable reference
        local,          // local variable reference
        float32,        // immediate 32-bit floating point
        float16i,       // immediate floating point packed into 16-bit integer
        string64,       // immediate 8 character string
    };

    struct operand
    {
        operand_type    type;
        std::uint8_t    offset;
        std::uint8_t    size;
        union
        {
            std::int8_t     value_int8;
            std::int16_t    value_int16;
            std::int32_t    value_int32;
            std::int64_t    value_int64;
            float           value_float32;
            char            value_string64[8];
        };
    };

    auto to_int(operand const & op, std::int32_t & value) noexcept -> bool;
    auto to_float(operand const & op, float & value) noexcept -> bool;

    enum instruction_flag : std::uint8_t
    {
        instruction_flag_not    = 1 << 0, // condition inversion
    };

    // instruction is a command instance with actual runtime values
    struct instruction
    {
        command const *     command;
        std::uint32_t       address;
        std::uint16_t       opcode;
        std::uint8_t        flags;
        std::uint8_t        size;
        std::uint8_t        operand_count;
        operand             operand_list[24];
    };

    auto name(instruction const & ins) noexcept -> char const *;
}
