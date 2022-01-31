# pragma once
# include <engine/engine.hpp>
# include <engine/operand.hpp>

namespace idascm
{
    class json_object;
    class json_primitive;
    class json_value;

    // NOTE:
    // - variable type (local, global) implies value modification
    // - constant type implies immediate value (not a variable)
    // TODO:
    // - think about variable_constant_xxx
    enum class type : std::uint8_t
    {
        unknown             = 0x0,
        any                 = 0x1,
        integer             = 0x2,                          // Signed integer
        real                = 0x3,                          // Floating point
        string              = 0x4,
        variadic            = 0x5,
        constant            = 0x6,                          // Any immediate value operand
        variable            = 0x7,                          // Any variable
        global              = 0x8,                          // Global variable (data address)
        local               = 0x9,                          // Local variable (register index)
        address             = 0xa       | (integer   << 4), // Any executable address (label or sub)
        mission             = 0xb       | (integer   << 4), // Mission ID (can be translated to segment address)
        script              = 0xc       | (integer   << 4), // External script ID (GTA:SA)
        constant_integer    = constant  | (integer   << 4),
        constant_real       = constant  | (real      << 4),
        constant_string     = constant  | (string    << 4),
        variable_integer    = variable  | (integer   << 4),
        variable_real       = variable  | (real      << 4),
        variable_string     = variable  | (string    << 4),
        global_integer      = global    | (integer   << 4), // VAR_INT
        global_real         = global    | (real      << 4), // VAR_FLOAT
        global_string       = global    | (string    << 4), // (GTASA)
        local_integer       = local     | (integer   << 4), // LVAR_INT
        local_real          = local     | (real      << 4), // LVAR_FLOAT
        local_string        = local     | (string    << 4), // (GTASA)
    };
    auto type_from_string(char const * string) noexcept -> type;
    auto type_from_json(json_primitive const & value) noexcept -> type;

    constexpr
    auto is_reference(type src) noexcept -> bool
    {
        switch (static_cast<type>(to_uint(src) & 0xf))
        {
            case type::variable:
            case type::local:
            case type::global:
                return true;
        }
        return false;
    }

    constexpr
    auto is_constant(type src) noexcept -> bool
    {
        return type::constant == static_cast<type>(to_uint(src) & 0xf);
    }

    constexpr
    auto remove_reference(type src) noexcept
    {
        return is_reference(src) ? static_cast<type>(to_uint(src) >> 4) : src;
    }

    constexpr
    auto remove_constant(type src) noexcept
    {
        return is_constant(src) ? static_cast<type>(to_uint(src) >> 4) : src;
    }

    struct argument
    {
        type            type;
        operand_type    operand_type;
        std::string     name;
    };
    auto argument_from_json(json_value const & value) noexcept -> argument;

    auto operator == (argument const & first, argument const & second) noexcept -> bool;
    auto operator != (argument const & first, argument const & second) noexcept -> bool;
}
