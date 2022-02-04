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
        command_flag_stop           = 1 <<  0, // Don't execute next instruction
        command_flag_branch         = 1 <<  1, // Branch execution flow
        command_flag_call           = 1 <<  2, // Function call
        command_flag_return         = 1 <<  3, // 
        command_flag_condition      = 1 <<  4, // Change condition flag
        command_flag_conditional    = 1 <<  5, // Use condition flag
        command_flag_function_call  = 1 <<  6, // Function call with arguments (GTA:LCS/VCS call_func)
        command_flag_unsupported    = 1 <<  7, // Execution is not possible
        command_flag_macro          = 1 <<  8, // Compiler internal high-level command (e.g. "{")
        command_flag_switch         = 1 <<  9, // Switch idiom (GTA:SA)
        command_flag_dependent      = 1 << 10, // Instruction depends on previous one (e.g. switch continue)
        command_flag_script_name    = 1 << 11, // Unique script name
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
