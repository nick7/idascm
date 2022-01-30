# pragma once
# include <engine/engine.hpp>

namespace idascm
{
    class json_primitive;

    struct command;

    enum class operand_type : std::uint8_t
    {
        unknown,            // Invalid Type
        none,               // Void type (variadic list end)
        local,              // Local variable reference
        global,             // Global variable reference
        timer,              // special local variable type
        local_array,        // Local variable array reference (GTA:SA, GTA:LCS, GTA:VCS)
        global_array,       // Global variable array reference (GTA:SA, GTA:LCS, GTA:VCS)
        string,             // Variable size string (GTA:SA)
        int0,               // Immediate 0i constant (GTA:LCS, GTA:VCS)
        int8,               // Immediate signed 8-bit integer
        int16,              // Immediate signed 16-bit integer
        int32,              // Immediate signed 32-bit integer
        int64,              // Immediate signed 64-bit integer (unused)
        uint8,              // (unused)
        uint16,             // (unused)
        uint32,             // (unused)
        uint64,             // (unused)
        float0,             // Immediate 0.f constant (GTA:LCS, GTA:VCS)
        float8p,            // Packed float
        float16p,           // Packed float
        float16i,           // immediate floating point packed into 16-bit integer
        float24p,           // Packed float
        float32,            // immediate 32-bit floating point
        float64,            // (unused)
        string8,            // immediate 8 character string
        string16,           // immediate 16 character string (GTA:SA)
        string128,          // immediate 128 character string (GTA:SA)
    };
    auto operand_type_from_string(char const * string) noexcept -> operand_type;
    auto operand_type_to_string(operand_type type) noexcept -> char const *;
    auto operand_type_from_json(json_primitive const & value) noexcept -> operand_type;
    auto operand_type_suffix(operand_type type) noexcept -> char const *;
    auto operand_type_is_signed(operand_type type) noexcept -> bool;

    enum class operand_array_flag : std::uint8_t
    {
        is_global       = 1 << 0, // (GTA:SA)
    };

    // Variable reference
    struct operand_variable
    {
        std::int32_t    address;    // data segment offset / register index
        operand_type    type;       // (optional) underlying type
    };

    // One dimensional array element reference (GTA:SA, GTA:LCS, GTA:VCS)
    struct operand_array
    {
        std::int32_t    address;        // data segment offset / register index
        std::uint16_t   index;
        std::uint8_t    size;
        operand_type    type    : 6;    // (optional) element type
        std::uint8_t    flags   : 2;    // (optional)
    };

    struct operand_string
    {
        std::int32_t    address;    // any segment address
        std::uint16_t   length;
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
            double              float64;
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
}
