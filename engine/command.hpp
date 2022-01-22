# pragma once
# include <engine/engine.hpp>
# include <engine/operand.hpp>
# include <string>

namespace idascm
{
    class json_object;
    class json_primitive;
    class json_value;

    // NOTE:
    // - variable type (local, global) means value may be changed by function
    enum class type : std::uint8_t
    {
        unknown             = 0x0,
        any                 = 0x1,
        integer             = 0x2,                          // signed integer
        real                = 0x3,                          // floating point
        character           = 0x4,
        string              = 0x5,
        variadic            = 0x6,
        variable            = 0x7,                          // any variable
        global              = 0x8,                          // global variable
        local               = 0x9,                          // local variable
        address             = 0xa,                          // code address (label or sub),
        variable_integer    = variable | (integer   << 4),
        variable_real       = variable | (real      << 4),
        variable_string     = variable | (string    << 4),
        global_integer      = variable | (integer   << 4),  // VAR_INT
        global_real         = variable | (real      << 4),  // VAR_FLOAT
        global_string       = variable | (string    << 4),
        local_integer       = variable | (integer   << 4),  // LVAR_INT
        local_real          = variable | (real      << 4),  // LVAR_FLOAT
        local_string        = variable | (string    << 4),
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
    auto remove_reference(type src) noexcept
    {
        return is_reference(src) ? static_cast<type>(to_uint(src) >> 4) : src;
    }

    struct argument
    {
        type            type;
        operand_type    operand_type;
    };
    auto argument_from_json(json_value const & value) noexcept -> argument;

    auto operator == (argument const & first, argument const & second) noexcept -> bool;
    auto operator != (argument const & first, argument const & second) noexcept -> bool;

    enum command_flag : std::uint16_t
    {
        command_flag_stop           = 1 <<  0, // terminate instruction flow
        command_flag_jump           = 1 <<  1, // simple jump
        command_flag_call           = 1 <<  2, // call function
        command_flag_return         = 1 <<  3, // 
        command_flag_condition      = 1 <<  4, // sets condition flag
        command_flag_conditional    = 1 <<  5, // uses condition flag
        command_flag_function_call  = 1 <<  6, // call function with arguments (LCS/VCS call_func)
        command_flag_unsupported    = 1 <<  7, // opcode is not supported by given executable
        command_flag_macro          = 1 <<  8, // compiler internal high-level command (e.g. "{")
        command_flag_cleo           = 1 << 15, // CLEO extension function
    };
    auto to_string(command_flag flag) noexcept -> char const *;

    // command is an instruction definition (specification) used by analyzer
    struct command
    {
        std::string     name;
        std::uint8_t    flags;
        std::uint8_t    argument_count;
        argument        argument_list[24];
        std::string     comment;
    };

    auto operator == (command const & first, command const & second) noexcept -> bool;

    auto command_from_json(json_object const & object) -> command;
}
