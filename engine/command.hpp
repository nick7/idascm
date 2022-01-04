# pragma once
# include <engine/engine.hpp>

namespace idascm
{
    class json_object;
    class json_value;

    enum class argument_type : std::uint8_t
    {
        unknown     = 0x0 << 4,

        // logical (abstract) types
        any         = 0x1 << 4,
        integer     = 0x2 << 4,     // signed integer
        real        = 0x3 << 4,     // floating point
        character   = 0x4 << 4,
        global      = 0x5 << 4,     // global variable
        local       = 0x6 << 4,     // local variable
        variadic    = 0x7 << 4,
        address     = 0x8 << 4,     // same as integer

        // fixed-size types
        int8        = integer   | 1, // 8-bit integer
        int16       = integer   | 2, // 16-bit integer
        int32       = integer   | 4, // 32-bit integer
        float32     = real      | 4,
        string64    = character | 8, // 8-byte string
    };
    auto argument_type_from_string(char const * string) noexcept -> argument_type;
    auto argument_type_from_json(json_value const & value) noexcept -> argument_type;

    enum command_flag : std::uint8_t
    {
        command_flag_stop           = 1 << 0,
        command_flag_jump           = 1 << 1,
        command_flag_call           = 1 << 2,
        command_flag_return         = 1 << 3,
        command_flag_condition      = 1 << 4, // sets condition flag
        command_flag_conditional    = 1 << 5, // uses condition flag
    };
    auto to_string(command_flag flag) noexcept -> char const *;
    
    // command is an instruction definition (specification) used by analyzer
    // TODO: move out opcode field
    struct command
    {
        char            name[64];
        std::uint8_t    flags;
        std::uint8_t    argument_count;
        argument_type   argument_list[16];
        char            comment[64];
    };

    auto operator == (command const & first, command const & second) noexcept -> bool;

    auto command_from_json(json_object const & object) -> command;
}
