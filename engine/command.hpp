# pragma once
# include <engine/engine.hpp>
# include <engine/argument.hpp>
# include <string>
# include <vector>

namespace idascm
{
    class json_object;

    enum command_flag : std::uint16_t
    {
        command_flag_stop           = 1 <<  0, // terminate instruction flow
        command_flag_branch         = 1 <<  1, // simple jump
        command_flag_call           = 1 <<  2, // call function
        command_flag_return         = 1 <<  3, // 
        command_flag_condition      = 1 <<  4, // sets condition flag
        command_flag_conditional    = 1 <<  5, // uses condition flag
        command_flag_function_call  = 1 <<  6, // call function with arguments (GTA:LCS/VCS call_func)
        command_flag_unsupported    = 1 <<  7, // opcode is not supported by given executable
        command_flag_macro          = 1 <<  8, // compiler internal high-level command (e.g. "{")
        command_flag_switch         = 1 <<  9, // switch instruction (GTA:SA)
        command_flag_dependent      = 1 << 10, // depends on previous one (e.g. switch continue)
        command_flag_cleo           = 1 << 15, // CLEO extension function
    };
    auto to_string(command_flag flag) noexcept -> char const *;

    // command is an instruction definition (specification) used by analyzer
    struct command
    {
        std::string             name;
        std::uint8_t            flags;
        std::vector<argument>   arguments;
        std::string             description;        // optional
        std::string             comment;            // optional
    };

    auto operator == (command const & first, command const & second) noexcept -> bool;

    auto command_from_json(json_object const & object) -> command;
}
