# pragma once
# include <engine/engine.hpp>
# include <string_view>

namespace idascm
{
    struct command;

    enum class operand_type : std::uint8_t
    {
        unknown,            // invalid type
        none,               // no type (variadic list end)
        local,              // local variable reference
        global,             // global variable reference
        timer,              // special local variable type
        local_array,
        global_array,
        string,             // variable size string
        int0,
        int8,               // immediate signed 8-bit integer
        int16,              // immediate signed 16-bit integer
        int32,              // immediate signed 32-bit integer
        int64,              // immediate signed 64-bit integer (reserved)
        float0,
        float8,             // packed float
        float16,            // packed float
        float16i,           // immediate floating point packed into 16-bit integer
        float24,            // packed float
        float32,            // immediate 32-bit floating point
        string8,            // immediate 8 character string
        string16,           // immediate 16 character string
    };

    // constexpr
    // auto to_operand_type(std::uint8_t type) noexcept
    // {
    //     return static_cast<operand_type>(type);
    // }

    enum operand_array_flag : std::uint8_t
    {
        operand_array_flag_is_global = 0x80,
    };

    struct operand_array
    {
        std::int32_t    address;
        std::uint16_t   index;
        std::uint8_t    size;
        std::uint8_t    flags;
    };

    struct operand_string
    {
        std::int32_t    address;
        std::uint16_t   length;
    };

    struct operand_variable
    {
        std::int32_t    address;
        operand_type    type;
    };

    struct operand_value
    {
        union
        {
            operand_array       array;
            operand_string      string;
            operand_variable    variable;
            std::int32_t        address;
            std::int8_t         int8;
            std::uint8_t        uint8;
            std::int16_t        int16;
            std::uint16_t       uint16;
            std::int32_t        int32;
            std::uint32_t       uint32;
            std::int64_t        int64;
            std::uint64_t       uint64;
            float               float32;
            char                string8[8];
        };
    };

    static_assert (sizeof(operand_value) <= 8);

    struct operand
    {
        operand_type    type;               // universal type
        std::uint8_t    type_internal;      // actual game type
        std::uint8_t    offset;
        std::uint8_t    size;               // total operand size
        operand_value   value;
    };

    auto to_int(operand const & op, std::int32_t & value) noexcept -> bool;
    auto to_float(operand const & op, float & value) noexcept -> bool;
    auto to_float(operand_type type, operand_value const & src, float & dst) noexcept -> bool;

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
        std::uint16_t       size;
        std::uint8_t        flags;
        std::uint8_t        operand_count;
        operand             operand_list[24];
    };

    auto instruction_name(instruction const & in) noexcept -> std::string_view;
}
